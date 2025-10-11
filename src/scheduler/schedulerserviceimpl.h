#include "../scheduler/schedulerservice.h"

class SchedulerServiceImpl : public SchedulerService {
    public:
        SchedulerServiceImpl() = default;
        JOB_STATUS getJobStatus(const std::string JobID);
        JOB_STATUS addJobToQueue(const std::string id, const json config, const BUILD_TYPE typeofBuild);
        int executeJob(const std::string id);

        int setJobStatus(const std::string id, const JOB_STATUS status);
        std::string enqueueDeployment (const std::string& repoURL, const json& config);
        void initWorkerPool() override;
        void workerThread() override;
        int nodejs_internal_build(const std::shared_ptr<Job> j);
        void removeJob(const std::shared_ptr<Job> j);
        std::shared_ptr<Job> getJobRef(const std::string id);
};  