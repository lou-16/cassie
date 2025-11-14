#ifndef SCHEDULERSERVICE_H
#define SCHEDULERSERVICE_H
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <optional>
#include <functional>
#include "json.hpp"


// This is the blueprint for the Scheduler service. it shall
/*
    -> Have a SchedulerService object that is to be default initialised
    -> Then anytime a particular project is to be built, read a cassie-build.json file
       and then build the project accordingly
    -> return values as per success:
       > 0 ; built successfully
       > 2 ; error in cassie-build.json file (could nt read)
       > 3 ; error in cassie-build.json file (incorrect syntax)
       *** extend as per requirements ***
*/

using json = nlohmann::json;

enum BUILD_TYPE {
    NOT_SPECIFIED,
    CPP,
    NODEJS,
};

enum JOB_STATUS{
    IDLE = 0,
    WORKING = 1,
    COMPLETED = 3,
    FAILED = 4,
    NOT_FOUND = 5
};

struct ContainerInfo;

typedef struct Job {

    std::string __id;
    json __config;
    BUILD_TYPE __build_type;
    JOB_STATUS __status;
    std::vector<std::reference_wrapper<ContainerInfo>> __containers;

    // temporary vector for container ids for serialization purposes
    std::vector<std::string> __containerIds;
    
    Job(const Job&) = default;
    Job(Job&&) noexcept = default;
    Job& operator=(const Job&) = default;
    Job& operator=(Job&&) noexcept = default;
    Job() = default;
}Job;

// for interal purposes, no such job needs to be created
inline Job emptyJob;
class SchedulerService {
    protected: 

        //internal Buffers : 

        static std::unordered_map<std::string, std::shared_ptr<Job>> Jobs;
        //Vector for active jobs (not processed yet.)
        static std::vector<std::shared_ptr<Job>> JobQueue; 
        static std::vector<std::thread> workers;

        //Locks and Mutexes
        static std::shared_mutex sharedMutex;

        //Condition variables
        static std::condition_variable_any workerThreadCV;
        static bool stop;
        virtual int setJobStatus() ;
        
        //Constants
        static const int MAX_THREADS = 10;

        // Worker Threads get called with this function. continuously checks for any jobs from jobAvailable variable
        virtual void workerThread(); //
        virtual ~SchedulerService() = default;  
    public:
        //SchedulerService();
        virtual void initWorkerPool(); // 
        virtual JOB_STATUS getJobStatus();
        virtual JOB_STATUS addJobToQueue();
        virtual int executeJob() ;
        virtual void Shutdown(); //
};

namespace ns {
    void to_json(nlohmann::json& jsonObj, const Job& j)
    {
        std::vector<std::string> containerIds;
        containerIds.reserve(j.__containers.size());
        for (const auto& ref : j.__containers)
        {
            containerIds.push_back(ref.get().id);
        }
        jsonObj = nlohmann::json{
            {"jobId", j.__id},
            {"config", j.__config},
            {"build_type", j.__build_type},
            {"containers", containerIds}
        }  ;
    }
    void from_json(nlohmann::json& j, Job& job)
    {
        j.at("jobId").get_to(job.__id);
        j.at("config").get_to(job.__config);
        j.at("build_type").get_to(job.__build_type);
        j.at("containers").get_to(job.__containerIds);
    }
}

#endif