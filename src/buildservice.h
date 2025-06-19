#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <shared_mutex>
// This is the blueprint for the build service. it shall
/*
    -> Have a BuildService object that is to be default initialised
    -> Then anytime a particular project is to be built, read a cassie-build.json file
       and then build the project accordingly
    -> return values as per success:
       > 0 ; built successfully
       > 2 ; error in cassie-build.json file (could nt read)
       > 3 ; error in cassie-build.json file (incorrect syntax)
       *** extend as per requirements ***
*/
enum BUILD_TYPE  {
    CPP,
    NODEJS,
};

enum JOB_STATUS{
    START = 0,
    WORKING = 1,
    COMPLETED = 3,
    FAILED = 4,
    NOT_FOUND = 5
};


typedef struct Job {
    const std::string __id;
    const std::string location;
    const BUILD_TYPE __build_type; 
}Job;

class BuildService {
    protected: 
        static std::unordered_map<std::string, JOB_STATUS> ActiveJobs;
        static std::vector<Job> JobQueue; 
        static std::shared_mutex sharedMutex;
        virtual ~BuildService() = default;    
        virtual int setJobStatus() ;
    public:
        virtual JOB_STATUS getJobStatus();
        virtual JOB_STATUS addJobToQueue();
        virtual int executeJob() ;
        
        
};