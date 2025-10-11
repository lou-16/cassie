#ifndef CONTAINERQUERIES_H
#define CONTAINERQUERIES_H

#include "../scheduler/schedulerserviceimpl.h"
#include "httplib.h"
#include "json.hpp"
#include <fstream>
#include "../utils/utils.h"
#include <memory>
#include "../utils/fetch.h"

using json = nlohmann::json;

typedef struct ContainerInfo {
    ContainerInfo() : null(true), workingDir("./"), jobInfo(nullptr){};
    bool null;
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
    
    //sorry for the weird naming i cant be bothered to come up with a better way for me to define these
    // std::ostream stdOUT;
    // std::istream stdIN;
    // std::ostream stdERR;
    
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

namespace ns 
{
    void to_json(json& j, const createContainerQuery& c)
    {
        j = json{
            {"Hostname" , c.Hostname},
            {"Domainame", c.Domainame},
            {"User", c.User},
            {"AttachStdin", c.AttachStdin},
            {"AttachStdOut", c.AttachStdOut},
            {"ExposedPorts", c.ExposedPorts},
            {"Tty", c.Tty},
            {"OpenStdin", c.OpenStdin},
            {"StdinOnce", c.StdinOnce},
            {"Env", c.Env},
            {"Cmd", c.Cmd},
            {"Healthcheck", c.Healthcheck},
            {"ArgsEscaped", c.ArgsEscaped || false},
            {"Image" , c.Image},
            {"Volumes", c.Volumes},
            {"WorkingDir", c.WorkingDir},
            {"Entrypoint", c.Entrypoint},
            {"NetworkDisabled", c.NetworkDisabled},
            {"MacAddress", c.MacAddress},
            {"OnBuild", c.Onbuild},
            {"Labels", c.Labels},
            {"StopSignal", c.StopSignal},
            {"StopTimeout", c.Stoptimeout},
            {"Shell", c.shell},
            {"HostConfig", c.HostConfig},
            {"NetworkingConfig", c.NetworkingConfig}
        };
    }
}

// TODO IMPLEMENT THIS FURTHER.
/* 
struct HostConfig {
    int CpuShares;
    uint64_t Memory;
    std::string CgroupParent;

}*/

#endif