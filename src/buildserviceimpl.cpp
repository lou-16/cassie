#include "buildserviceimpl.h"
#include "utils.h"
#include <thread>

int BuildServiceImpl::setJobStatus(const std::string id, const JOB_STATUS status)  {
    std::unique_lock lock(sharedMutex);
    if(status == JOB_STATUS::NOT_FOUND){
        return -1;
    }
    auto it = ActiveJobs.find(id);
    if(it != ActiveJobs.end()){
        it->second = status;
        return it->second;
    }   return -1;
}

JOB_STATUS BuildServiceImpl::getJobStatus(const std::string id)  {
    std::shared_lock lock(sharedMutex);
    auto it = ActiveJobs.find(id);
    if(it == ActiveJobs.end()){
        return JOB_STATUS::NOT_FOUND;
    } else {
        return it->second;
    }
}

JOB_STATUS BuildServiceImpl::addJobToQueue(const std::string id, const std::string location, const BUILD_TYPE typeOfBuild)  { 
    std::unique_lock lock(sharedMutex);
    Job _job = {id, location, typeOfBuild};
    JobQueue.push_back(_job); 
    return JOB_STATUS::START;
}

int BuildServiceImpl::executeJob(const std::string id)  {
    std::cout << "[+] building ... with id : " << id << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "[+] linking ...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "[+] Job Done LOL\n";
    return JOB_STATUS::COMPLETED;
}