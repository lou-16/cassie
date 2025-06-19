#include "buildservice.h"

class BuildServiceImpl : public BuildService {
    public:
        BuildServiceImpl() = default;
        JOB_STATUS getJobStatus(const std::string id);
        JOB_STATUS addJobToQueue(const std::string id, const std::string location, const BUILD_TYPE typeofBuild);
        int executeJob(const std::string id);

        int setJobStatus(const std::string id, const JOB_STATUS status);
};
