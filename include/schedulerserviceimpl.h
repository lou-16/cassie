#ifndef SCHEDULERSERVICEIMPL_H
#define SCHEDULERSERVICEIMPL_H

#include "schedulerservice.h"

class SchedulerServiceImpl : public SchedulerService {
    public:
        SchedulerServiceImpl() = default;
        JOB_STATUS getJobStatus(const std::string JobID);
        JOB_STATUS addJobToQueue(const std::string id, const json config, const BUILD_TYPE typeofBuild);
        int executeJob(const std::string id);

        int setJobStatus(const std::string id, const JOB_STATUS status);
        std::string enqueueDeployment (const std::string& repoURL, const json& config, const std::string& ProjectType);
        void initWorkerPool() override;
        void workerThread() override;
        int nodejs_internal_build(const std::shared_ptr<Job> j);
        void removeJob(const std::shared_ptr<Job> j);
        std::optional<std::reference_wrapper<Job>> getJobRef(const std::string& id);
        
};  
#endif