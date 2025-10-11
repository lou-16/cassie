#include "schedulerserviceimpl.h"
#include "../utils/utils.h"
#include <thread>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <memory>
using json = nlohmann::json;

#include "../git/gitserviceimpl.h"
// Set the job status in the active jobs map
int SchedulerServiceImpl::setJobStatus(const std::string id, const JOB_STATUS status)
{
    std::unique_lock lock(sharedMutex);
    if (status == JOB_STATUS::NOT_FOUND)
    {
        return -1;
    }
    auto it = Jobs.find(id);
    if (it != Jobs.end())
    {
        it->second->__status = status;
        return static_cast<int>(it->second->__status);
    }
    return -1;
}

// Get the current job status
JOB_STATUS SchedulerServiceImpl::getJobStatus(const std::string id)
{
    std::shared_lock lock(sharedMutex);
    auto it = Jobs.find(id);
    if (it == Jobs.end())
    {
        return JOB_STATUS::NOT_FOUND;
    }
    return it->second->__status;
}

// Add a new job to the queue
JOB_STATUS SchedulerServiceImpl::addJobToQueue(const std::string id, const std::string location, const BUILD_TYPE typeOfBuild)
{
    std::cout << "[+] addJobToQueue called";
    auto ptr = std::make_shared<Job>(Job{id, location, typeOfBuild});
    {
        std::unique_lock lock(uniqueLock);
        JobQueue.push_back(ptr);
        Jobs[id] = ptr;
    }
    workerThreadCV.notify_one();

    return JOB_STATUS::START;
}

// Execute a job from the queue based on ID
int SchedulerServiceImpl::executeJob(const std::string id)
{
    std::cout << "[+] executeJob Called : building ... with id: " << id << "\n";

    std::unique_lock lock(sharedMutex);
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [&](const std::shared_ptr<Job>& job)
                           { return job.get() -> __id == id; });

    if (it == JobQueue.end())
    {
        std::cout << "[-] Job not found in queue.\n";
        return -1;
    }

    Job jobToExecute = *it->get();

    // branch out the jobs based on build type
    switch (jobToExecute.__build_type)
    {
    case BUILD_TYPE::NODEJS:
        Jobs[jobToExecute.__id].get()->__status = JOB_STATUS::WORKING;
        nodejs_internal_build(*it);
        break;
    case BUILD_TYPE::CPP:
        // TODO: implement logic
        break;
    default:
        break;
    }

    // Mark job as completed
    Jobs[id].get()->__status = JOB_STATUS::COMPLETED;

    std::cout << "[+] Job executed successfully.\n";
    return static_cast<int>(JOB_STATUS::COMPLETED);
}

int SchedulerServiceImpl::nodejs_internal_build(const std::shared_ptr<Job> ptr)
{
    Job& j = *ptr;
    std::cout << "\n[+] Internal Builder called \n";
    std::string command = "cd" + j.location + " && " + "npm i";
    int result = std::system(command.c_str());

    if (result != 0)
    {
        Jobs[j.__id].get()->__status = JOB_STATUS::FAILED;
    }
    else
    {
        // TODO : add mechanism to store and add the url for the reverse proxy.
        Jobs[j.__id].get()->__status = JOB_STATUS::WORKING;
    }
}

void SchedulerServiceImpl::removeJob(const std::shared_ptr<Job> j)
{
    std::cout << "[+] RemoveJob called" << std::endl;
    std::unique_lock lock(uniqueLock);
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [j](const std::shared_ptr<Job> &x)
                           { return x.get()->__id == j.get()->__id; });
    if (it != JobQueue.end())
    {
        JobQueue.erase(it);
    }
    auto it2 = Jobs.find(j.get()->__id);
    if (it2 != Jobs.end())
    {
        Jobs.erase(j.get()->__id);
    }
    return;
}

//here the Job Object is created and addJobToQueue is called, which will add the jobs to the buffers
std::string SchedulerServiceImpl::enqueueDeployment(const std::string &RepoURL, const std::string &prefix)
{
    std::string targetDir = appendEpochTo(prefix);
    GitServiceImpl git;
    if (!git.handleRequest(RepoURL, targetDir))
    {
        throw std::runtime_error("git clone failed");
    }

    json settings;
    std::string filePath = targetDir + "/cassie-config.json";
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Scheduler Error: cassie-config.json not found at " + filePath);
    }

    try {
        file >> settings;
    } catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("Scheduler Error: error parsing cassie-config.json: ") + e.what());
    }

    // validate settings
    if (!settings.contains("project_type") || !settings["project_type"].is_string()) {
        throw std::runtime_error("Scheduler Error: cassie-config.json missing 'project_type' string");
    }

    std::string id = createUniqueId();
    BUILD_TYPE currType = [&settings]() -> BUILD_TYPE
    {
        std::string type = settings["project_type"];
        if (type == "c++" || type == "c" || type == "cpp")
        {
            return BUILD_TYPE::CPP;
        }
        if (type == "nodejs" || type == "NODEJS" || type == "node" || type == "NODE")
        {
            return BUILD_TYPE::NODEJS;
        }
        else
        {
            return BUILD_TYPE::NOT_SPECIFIED;
        }
    }();
    addJobToQueue(id, targetDir, currType);
    return id;
}

void SchedulerServiceImpl::initWorkerPool()
{
    std::cout << "initialised workerThread Pool" << std::endl;
    for (int i = 0; i < MAX_THREADS; i++)
    {
        workers.emplace_back(std::thread(&SchedulerServiceImpl::workerThread, this));
    }
}

void SchedulerServiceImpl::workerThread()
{
    while (!stop)
    {
        std::unique_lock lock(uniqueLock);

        workerThreadCV.wait(lock, []
            { 
                auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [](const std::shared_ptr<Job> &j){
                    return !JobQueue.empty() || stop;
                });
            });                                      

        // Now we are guaranteed there's a job
        Job& _j = *(JobQueue.front());
        lock.unlock(); // release mutex before long work

        executeJob(_j.__id);
        removeJob(JobQueue.front());
    }
}

std::shared_ptr<Job> SchedulerServiceImpl::getJobRef(const std::string id)
{
    std::unique_lock lock(uniqueLock);
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [id](const std::shared_ptr<Job> &j)
                           { return j.get()->__id == id; });
    std::shared_ptr<Job> ptr = (it != JobQueue.end())? *it : std::shared_ptr<Job>();
    if (ptr)
    {
        return ptr;
    }
    else
    {
        return std::shared_ptr<Job>();
    }
}
