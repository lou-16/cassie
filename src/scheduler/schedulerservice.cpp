#include "schedulerservice.h"

std::unordered_map<std::string, std::shared_ptr<Job>> SchedulerService::Jobs;
std::vector<std::shared_ptr<Job>> SchedulerService::JobQueue;
std::shared_mutex SchedulerService::sharedMutex;
std::mutex SchedulerService::uniqueLock;
std::condition_variable SchedulerService::workerThreadCV;
bool SchedulerService::stop = false;
std::vector<std::thread> SchedulerService::workers;
const int SchedulerService::MAX_THREADS;


int SchedulerService::setJobStatus()
{
    return 0;
}

void SchedulerService::workerThread()
{
}

void SchedulerService::initWorkerPool()
{
}

JOB_STATUS SchedulerService::getJobStatus()
{
    return JOB_STATUS();
}

JOB_STATUS SchedulerService::addJobToQueue()
{
    return JOB_STATUS();
}

int SchedulerService::executeJob()
{
    return 0;
}

void SchedulerService::Shutdown()
{
}
