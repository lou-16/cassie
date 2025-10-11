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
JOB_STATUS SchedulerServiceImpl::addJobToQueue(const std::string id, const json config, const BUILD_TYPE typeOfBuild)
{
    //std::cout << "[+] addJobToQueue called";
    /* makes a shared pointer for a job object*/
    auto ptr = std::make_shared<Job>(Job{id, config, typeOfBuild, JOB_STATUS::IDLE});
    {
        /* modifies the internal buffers */
        std::unique_lock lock(uniqueLock);
        JobQueue.push_back(ptr);
        Jobs[id] = ptr;
    }
    /* notifies the worker threads, so one of them wakes up for processing */
    workerThreadCV.notify_one();

    return JOB_STATUS::IDLE;
}

// Execute a job from the queue based on ID
int SchedulerServiceImpl::executeJob(const std::string id)
{
    // std::cout << "[+] executeJob Called : building ... with id: " << id << "\n";

    /* locking the mutex */
    std::unique_lock lock(sharedMutex);

    /* finds the iterator to a particular job given its JobID (named id here) using a find_if function */
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [&](const std::shared_ptr<Job>& job)
                           { return job.get() -> __id == id; });

    /* checks if the iterator is valid */
    if (it == JobQueue.end())
    {
        //std::cout << "[-] Job not found in queue.\n";
        return 1;
    }

    /* derefs the iterator, getting the shared ptr, and then calls get to get the ref to the Job object*/
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

    return static_cast<int>(JOB_STATUS::COMPLETED);
}

// TODO (IMPORTANT) : function needs a rewrite, call something from ContainerService. moving on.
/*
int SchedulerServiceImpl::nodejs_internal_build(const std::shared_ptr<Job> ptr)
{
    
    // std::cout << "\n[+] Internal Builder called \n";
    
    // TODO : make this better by spawing a process 
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
*/

void SchedulerServiceImpl::removeJob(const std::shared_ptr<Job> j)
{
    /* locking the mutex */
    std::unique_lock lock(uniqueLock);
    /* using a find_if to get the iterator*/
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [j](const std::shared_ptr<Job> &x)
                           { return x.get()->__id == j.get()->__id; });
    /* iterator validity check */
    if (it != JobQueue.end())
    {   
        /* method to remove a value from the Queue*/
        JobQueue.erase(it);
    }
    /* iterator for the map */
    auto it2 = Jobs.find(j.get()->__id);
    /* validity check */
    if (it2 != Jobs.end())
    {
        /* erase from map */
        Jobs.erase(j.get()->__id);
    }
    return;
}

/* here the Job Object is created and addJobToQueue is called, which will add the jobs to the buffers
   also, this is basically what the router is supposed to do. so i will write it with that in mind
   although, i just realised, the map will track which user has the job associated. (unrelated lol)
   -> future me, no! the map is for quick IO for the backend to return to. the responsibility of 
      tracking the userID -> jobID relation is of both:
      > the database (backend)
      > the redis cache (backend)

*/
std::string SchedulerServiceImpl::enqueueDeployment(const std::string &RepoURL, const json& config)
{   
    /* done. that was relatively quick */

    /* create a new ID for JobID purposes */
    std::string id = createUniqueId();
    BUILD_TYPE currType = [&config]() -> BUILD_TYPE
    {
        std::string type = config["project_type"];
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
    addJobToQueue(id, config, currType);
    return id;
}

void SchedulerServiceImpl::initWorkerPool()
{
    //std::cout << "initialised workerThread Pool" << std::endl;
    /* for loop places newly created threads, which use the workerThread func as their function arg */
    for (int i = 0; i < MAX_THREADS; i++)
    {   
        /* since we are outside the class, we need to provide a ref to the object, this */
        workers.emplace_back(std::thread(&SchedulerServiceImpl::workerThread, this));
    }
}

void SchedulerServiceImpl::workerThread()
{
    while (!stop)
    {
        std::unique_lock lock(uniqueLock);

        /* wait takes in a lock that needs to be locked before any work is done, and a predicate that needs 
           to return true */
        workerThreadCV.wait(lock, []
            { 
                    /* checks if JobQueue has an element */
                    return !JobQueue.empty() || stop;
            });                                      
        /* check if stop is true, since even if it is true, the whole while block will finish exec first
           so we continue to let this iteration end
         */
        if(stop) {continue;}
        
        // Now we are guaranteed there's a job
        Job& _j = *(JobQueue.front());
        lock.unlock(); // release mutex before long work

        /* called executeJob */
        executeJob(_j.__id);
        /* remove job after it has been worked upon */
        removeJob(JobQueue.front());
    }
}

/* this works, comments will be done later */
std::shared_ptr<Job> SchedulerServiceImpl::getJobRef(const std::string id)
{
    /* locks the map */
    std::unique_lock lock(uniqueLock);
    
    /* finds the iterator using find_if*/
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [id](const std::shared_ptr<Job> &j)
                           { return j.get()->__id == id; });
    
    /* creates a pointer, then checks if iterator is valid. if yes, then deref the iterator, if no, then an empty pointer*/
    return (it != JobQueue.end())? *it : std::shared_ptr<Job>();
    
}
