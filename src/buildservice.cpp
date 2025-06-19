#include "buildservice.h"

std::unordered_map<std::string, JOB_STATUS> BuildService::ActiveJobs;
std::vector<Job> BuildService::JobQueue;
std::shared_mutex BuildService::sharedMutex;

int BuildService::setJobStatus()
{
    return 0;
}

JOB_STATUS BuildService::getJobStatus()
{
    return JOB_STATUS();
}

JOB_STATUS BuildService::addJobToQueue()
{
    return JOB_STATUS();
}

int BuildService::executeJob()
{
    return 0;
}
