#include "buildserviceimpl.h"

int BuildServiceImpl::setJobStatus(const std::string id, const JOB_STATUS status) override
{
    try{
        auto iterator = ActiveJobs.find(id);
        if(iterator == ActiveJobs.end()){
            throw int(1);
        }
        else {
            ActiveJobs[id] = status;
        }
    } catch (int i) {
        if(i == 1){
            return -1;
        }
    }
}
