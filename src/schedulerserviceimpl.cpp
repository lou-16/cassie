#include "schedulerserviceimpl.h"
#include "utils.h"
#include <thread>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

#include "gitserviceimpl.h"
// Set the job status in the active jobs map
int SchedulerServiceImpl::setJobStatus(const std::string id, const JOB_STATUS status) {
    std::unique_lock lock(sharedMutex);
    if (status == JOB_STATUS::NOT_FOUND) {
        return -1;
    }
    auto it = ActiveJobs.find(id);
    if (it != ActiveJobs.end()) {
        it->second = status;
        return static_cast<int>(it->second);
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
    return it->second;
}

// Add a new job to the queue
JOB_STATUS SchedulerServiceImpl::addJobToQueue(const std::string id, const std::string location, const BUILD_TYPE typeOfBuild) {
    std::unique_lock lock(sharedMutex);
    Job job = {id, location, typeOfBuild};
    JobQueue.push_back(job);
    ActiveJobs[id] = JOB_STATUS::START;

    

    return JOB_STATUS::START;
}

// Execute a job from the queue based on ID
int SchedulerServiceImpl::executeJob(const std::string id) {
    std::cout << "[+] building ... with id: " << id << "\n";
    
    std::unique_lock lock(sharedMutex);
    auto it = std::find_if(JobQueue.begin(), JobQueue.end(), [&](const Job& job) {
        return job.__id == id;
    });

    if (it == JobQueue.end()) {
        std::cout << "[-] Job not found in queue.\n";
        return -1;
    }

    Job jobToExecute = *it;

    // Simulate job execution here
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Fake build delay

    // Mark job as completed
    ActiveJobs[id] = JOB_STATUS::COMPLETED;

    std::cout << "[+] Job executed successfully.\n";
    return static_cast<int>(JOB_STATUS::COMPLETED);
}

std::string SchedulerServiceImpl::enqueueDeployment(const std::string& RepoURL, const std::string& prefix) {
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
    } catch (const std::runtime_error& e) {
        throw new std::runtime_error("[+] Scheduler Error : InvalidConfigException");
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
    }();
    addJobToQueue(id, targetDir, currType);

}

void SchedulerServiceImpl::initWorkerPool() {
    for (int i = 0; i < MAX_THREADS ; i ++){
        workers.emplace_back(std::thread(workerThread));
    }
}

void SchedulerServiceImpl::enqueueTask(std::function<void()> task) {
    {
        std::unique_lock lock(taskQueue);
        taskQueue.push(std::move(task));
    }
    jobAvailable.notify_one();
}