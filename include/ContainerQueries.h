#ifndef CONTAINERQUERIES_H
#define CONTAINERQUERIES_H

#include "schedulerserviceimpl.h"
#include "httplib.h"
#include "json.hpp"
#include <fstream>
#include "utils.h"
#include <memory>
#include "fetch.h"

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

#endif