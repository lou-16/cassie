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


typedef struct Job {

    std::string __id;
    json config;
    BUILD_TYPE __build_type;
    JOB_STATUS __status;
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
        static std::mutex uniqueLock;

        //Condition variables
        static std::condition_variable workerThreadCV;
        static bool stop;
        virtual int setJobStatus() ;
        
        //Constants
        static const int MAX_THREADS = 10;

        // Worker Threads get called with this function. continuously checks for any jobs from jobAvailable variable
        virtual void workerThread(); //
        virtual ~SchedulerService() = default;  
    public:
        virtual void initWorkerPool(); // 
        virtual JOB_STATUS getJobStatus();
        virtual JOB_STATUS addJobToQueue();
        virtual int executeJob() ;
        virtual void Shutdown(); //
};

#endif