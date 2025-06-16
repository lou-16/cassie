#include "buildservice.h"

class BuildServiceImpl : public BuildService {
    public:
        JOB_STATUS getJobStatus(const std::string id);
        int addJobToQueue(const std::string location, const BUILD_TYPE typeofBuild);
        int executeJob(const std::string id);

};
