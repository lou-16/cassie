#include "../scheduler/schedulerservice.h"

class SchedulerServiceImpl : public SchedulerService {
    public:
        SchedulerServiceImpl() = default;
        JOB_STATUS getJobStatus(const std::string id);
        JOB_STATUS addJobToQueue(const std::string id, const std::string location, const BUILD_TYPE typeofBuild);
        int executeJob(const std::string id);

        int setJobStatus(const std::string id, const JOB_STATUS status);
        std::string enqueueDeployment (const std::string& repoURL, const std::string& prefix);
        void initWorkerPool() override;
        void workerThread() override;
        void nodejs_internal_build(const Job&);
        void removeJob(const Job&);
        Job& getJobRef(const std::string id);
};  