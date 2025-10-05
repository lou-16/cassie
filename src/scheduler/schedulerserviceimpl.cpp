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
int SchedulerServiceImpl::setJobStatus(const std::string id, const JOB_STATUS status) {
    std::unique_lock lock(sharedMutex);
    if (status == JOB_STATUS::NOT_FOUND) {
        return -1;
    }
    auto it = ActiveJobs.find(id);
    if (it != ActiveJobs.end()) {
        it->second->__status = status;
        return static_cast<int>(it->second->__status);
    }
    return -1;
}

// Get the current job status
JOB_STATUS SchedulerServiceImpl::getJobStatus(const std::string id) {
    std::shared_lock lock(sharedMutex);
    auto it = ActiveJobs.find(id);
    if (it == ActiveJobs.end()) {
        return JOB_STATUS::NOT_FOUND;
    }
    return it->second->__status;
}

// Add a new job to the queue
JOB_STATUS SchedulerServiceImpl::addJobToQueue(const std::string id, const std::string location, const BUILD_TYPE typeOfBuild) {
    std::cout << "[+] addJobToQueue called";
    Job job = {id, location, typeOfBuild};
    {
        std::unique_lock lock(uniqueLock);
        JobQueue.push_back(std::move(job));
        ActiveJobs[id] = (JobQueue.end() - 1)->;
    }
    jobAvailable.notify_one();

    return JOB_STATUS::START;
}

// Execute a job from the queue based on ID
int SchedulerServiceImpl::executeJob(const std::string id) {
    std::cout << "[+] executeJob Called : building ... with id: " << id << "\n";
    
    std::unique_lock lock(sharedMutex);
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [&](const Job& job) {
        return job.__id == id;
    });

    if (it == JobQueue.end()) {
        std::cout << "[-] Job not found in queue.\n";
        return -1;
    }

    Job jobToExecute = *it;

    // branch out the jobs based on build type
    switch (jobToExecute.__build_type)
    {
    case BUILD_TYPE::NODEJS:
        ActiveJobs[jobToExecute.__id] = JOB_STATUS::WORKING;
        nodejs_internal_build(jobToExecute);
        break;
    case BUILD_TYPE::CPP:
        // TODO: implement logic 
        break;
    default:
        break;
    }

    // Mark job as completed
    ActiveJobs[id] = JOB_STATUS::COMPLETED;

    std::cout << "[+] Job executed successfully.\n";
    return static_cast<int>(JOB_STATUS::COMPLETED);
}

void SchedulerServiceImpl::nodejs_internal_build(const Job& j){
    std::cout << "\n[+] Internal Builder called \n";
    std::string command = "cd" + j.location + "&&"+ "npm i";
    int result = std::system(command.c_str());

    if(result != 0) {
        ActiveJobs[j.__id] = JOB_STATUS::FAILED;
    } else {
        // TODO : add mechanism to store and add the url for the reverse proxy.
        ActiveJobs[j.__id] = JOB_STATUS::WORKING;
        removeJob(j);
    }
}

void SchedulerServiceImpl::removeJob(const Job& j) {
    std::cout << "[+] RemoveJob called" << std::endl;
    std::unique_lock lock(uniqueLock);
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [j](const Job& x) {
        return x.__id == j.__id;
    });
    if(it != JobQueue.end()){
        JobQueue.erase(it);
    }
    auto it2 = ActiveJobs.find(j.__id);
    if(it2 != ActiveJobs.end()){
        ActiveJobs.erase(j.__id);
    }
    return;
}

std::string SchedulerServiceImpl::enqueueDeployment(const std::string& RepoURL, const std::string& prefix) {
    std::cout << "\n[+] enqueueDeployment Called" << std::endl;
    std::string targetDir = appendEpochTo(prefix);
    GitServiceImpl git;
    if(!git.handleRequest(RepoURL, targetDir)) {
        throw new std::runtime_error("git clone failed");
    }
    json settings;
    try {
        if (targetDir == "") {
            throw new std::runtime_error ("[+] Scheduler Error : something failed and targetDir is empty. CRITICAL ERROR, pls see");
        }
        std::string filePath = targetDir + "/cassie-config.json";
        std::ifstream file(filePath);
        if(!file.is_open()){
            throw new std::runtime_error("[+] Scheduler Error : cassie-config.json not found");
        }
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e ) {
            throw new std::runtime_error("[+] Scheduler Error : error parsing cassie-config.json. check for syntax errors");
        }
        settings = j;
    } catch (const std::runtime_error e) {
        std::cout <<"[+] Scheduler Error : InvalidConfigException" << e.what();
    }

    std::string id = createUniqueId();
    BUILD_TYPE currType = [&settings]() -> BUILD_TYPE {
        std::string type = settings["project_type"];
        if(type == "c++" || type == "c" || type == "cpp"){
            return BUILD_TYPE::CPP;
        }
        if(type == "nodejs" || type =="NODEJS" || type == "node" || type == "NODE"){
            return BUILD_TYPE::NODEJS;
        }
        else {
            return BUILD_TYPE::NOT_SPECIFIED;
        }
    }();
    ActiveJobs[id] = JOB_STATUS::START;
    addJobToQueue(id, targetDir, currType);
    return id;
}

void SchedulerServiceImpl::initWorkerPool() {
    std::cout << "initialised workerThread Pool" << std::endl;
    for (int i = 0; i < MAX_THREADS ; i ++){
        workers.emplace_back(std::thread(workerThread, this));
    }
}

void SchedulerServiceImpl::workerThread() {
    std::cout << "\n[+] workerThread Called\n";
    while (!stop) {
        std::unique_lock lock(uniqueLock);

        jobAvailable.wait(lock, [] {
            auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [](const std::shared_ptr<Job>& j){
                return j.get()->
            })
            return !SchedulerServiceImpl::JobQueue.empty();
        });

        // Now we are guaranteed there's a job
        Job _j = SchedulerService::JobQueue.front();
        lock.unlock();  // release mutex before long work

        std::cout << "[+] workerThread executing job given\n";
        executeJob(_j.__id);        
        removeJob(_j);
    }
}

Job& SchedulerServiceImpl::getJobRef(const std::string id){
    std::unique_lock lock(uniqueLock);
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [id](const Job& j) {
        return j.__id == id;
    });
    if(it != JobQueue.end()){
        return *it;
    } else {
        return emptyJob;
    }
}
