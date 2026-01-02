#pragma once
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
#include <unordered_set>

#include "json.hpp"
#include "JSONSerialization.h"

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

       REWRITE.
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
    std::unordered_set<std::string> __containers;

    // temporary vector for container ids for serialization purposes
    std::vector<std::string> __containerIds;
    
    Job(const Job&) = default;
    Job(Job&&) noexcept = default;
    Job& operator=(const Job&) = default;
    Job& operator=(Job&&) noexcept = default;
    Job() = default;
    
}Job;


// for interal purposes, no such job needs to be created
class SchedulerService {
    protected: 

        //internal Buffers : 

        static std::unordered_map<std::string, JOB_STATUS> Jobs;
        //Vector for active jobs (not processed yet.)
        static std::deque<std::unique_ptr<Job>> JobQueue; 
        static std::vector<std::thread> workers;

        //Locks and Mutexes
        static std::shared_mutex sharedMutex;

        //Condition variables
        static std::condition_variable_any workerThreadCV;
        static bool stop;
        int setJobStatus() ;
        
        //Constants
        static const int MAX_THREADS = 10;

        // Worker Threads get called with this function. continuously checks for any jobs from jobAvailable variable
        void workerThread(); //
          
    public:
        //SchedulerService();
        ~SchedulerService() = default;
        void initWorkerPool(); // 
        void Shutdown(); //
        JOB_STATUS getJobStatus(const std::string JobID);
        JOB_STATUS addJobToQueue(const std::string id, const json config, const BUILD_TYPE typeofBuild);
        int executeJob(const std::string id);

        int setJobStatus(const std::string id, const JOB_STATUS status);
        std::string enqueueDeployment (const std::string& repoURL, const json& config, const std::string& ProjectType);
        int nodejs_internal_build(const std::shared_ptr<Job> j);
        void removeJob(const std::shared_ptr<Job> j);
        // Non Owning Ptr.
        Job* getJobRef(const std::string& id);
};

