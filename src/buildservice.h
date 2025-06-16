#pragma once
#include <string>
#include <vector>
#include <unordered_map>

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
const enum BUILD_TYPE  {
    CPP,
    NODEJS,
};

constexpr enum JOB_STATUS{
    READY = 0,
    WORKING = 1,
    SUSPENDED = 2,
    COMPLETED = 3,
    FAILED = 4
};


struct Job {
    const std::string __id;
    const std::string location;
    const BUILD_TYPE __build_type; 
};

class BuildService {
    protected: 
        std::unordered_map<std::string, JOB_STATUS> ActiveJobs;
    private:    
        virtual ~BuildService() = default;
        virtual int setJobStatus() = 0;
    public:
        virtual JOB_STATUS getJobStatus();
        virtual int addJobToQueue();
        virtual int executeJob() = 0;
        
        
};