#ifndef CONTAINERSERVICE_H
#define CONTAINERSERVICE_H
#include <thread>
#include <fstream>
#include <memory>
#include <streambuf>
#include <unistd.h>

#include "httplib.h"
#include "json.hpp"
#include "fetch.h"
#include "utils.h"
#include "schedulerservice.h"
#include <optional>
#include <variant>



using json = nlohmann::json;

struct Job; 

typedef struct ContainerInfo {
    ContainerInfo() : status(true), workingDir("./"), jobInfo(nullptr){};
    bool status;
    std::string id = "";
    std::string jobId;
    std::string Image;
    std::string workingDir;
    std::vector<std::string> copyPaths;
    std::shared_ptr<Job> jobInfo;
    std::vector<std::string> aptPkgs;

    std::vector<std::string> buildCommands;

    std::string entryPoint;
    std::vector<std::string> cmd;

    std::vector<int> exposedPorts;
    std::map <std::string, std::string> envVars;

    std::vector<std::string> volumes;
}ContainerInfo;

struct createContainerQuery {
    std::string Hostname = "";
    std::string Domainame = "";
    std::string User = "";
    bool AttachStdin;
    bool AttachStdOut;
    bool AttachStderr;
    json ExposedPorts;
    bool Tty;
    bool OpenStdin;
    bool StdinOnce;
    // Array of strings please. please.
    json Env;
    // Array of commands(strings), json.
    json Cmd;
    json Healthcheck;
    bool ArgsEscaped;
    std::string Image;
    json Volumes;
    std::string WorkingDir;
    json Entrypoint;
    bool NetworkDisabled;
    std::string MacAddress;
    json Onbuild;
    json Labels;
    std::string StopSignal = "SIGTERM";
    int Stoptimeout = 10;// in  seconds
    json shell; // array of strings of shells to use for RUN, CMD, and ENTRYPOINT
    json HostConfig;
    json NetworkingConfig; 
};


// TODO IMPLEMENT THIS FURTHER.
/* 
struct HostConfig {
    int CpuShares;
    uint64_t Memory;
    std::string CgroupParent;

}*/


class ContainerService {
    private:
        httplib::Client client;
    public:
        /* TODO : Switch this out for a non-blocking socket I/O lib */
        ContainerService() : client("unix://var/run/docker.sock") {
            client.set_connection_timeout(5); //seconds
            client.set_read_timeout(5);
            client.set_write_timeout(5);
            if(client.is_valid()){
                std::cout << "connected to docker socket\n";
            }
        };
        /* key : containerId, key : unique_ptr to the container associated  */
        static std::unordered_map<std::string,std::unique_ptr<ContainerInfo>> containersMap;
        static std::shared_mutex containersMutex; /* mutex for the containersMap */
     
        /* class methods / API for communication with the Docker UNIX socket */
        

        /* API */
        ContainerInfo* findContainer (const std::string& JobID); //
        bool startContainer                         (const std::string& JobID); //
        bool stopContainer                          (const std::string& JobID); //
        std::string getContainerStatus              (const std::string& JobID); //
        ContainerInfo* createContainer(const std::string& JobID);

        /* destructor TODO : delete/free all ptrs to the containers. this will cause a memory leak if not fixed */
        ~ContainerService() = default; // 

        /* this adds the unique ptr to container to the map TODO: design this better later */
        void initialiseContainerInfo(Job& J, const json& config); // 

        /* TODO (IMPORTANT) : extend this class to allow for TCP streams to be sent to the backend */
};

#endif /* CONTAINERSERVICE_H*/