#pragma once 

#include <thread>
#include "schedulerservice.h"
#include "httplib.h"
#include "json.hpp"
#include <fstream>
#include "utils.h"
#include <memory>

using json = nlohmann::json;

typedef struct ContainerInfo {
    ContainerInfo() : null(true), workingDir("./"), jobInfo(){  };
    bool null;
    std::string id = "container_" + ((jobInfo == nullptr || jobInfo->__id == ""  )? "null" : jobInfo->__id);
    std::string name;
    std::string Image;
    std::string workingDir;
    std::vector<std::string> copyPaths;
    std::shared_ptr<Job> jobInfo;
    std::vector<std::string> aptPkgs;

    std::vector<std::string> b_commands;

    std::string entryPoint;
    std::vector<std::string> cmd;

    std::vector<int> exposedPorts;
    std::map <std::string, std::string> envVars;

    std::vector<std::string> volumes;
    
}ContainerInfo;

class ContainerService {
    public:
        static std::vector<std::shared_ptr<ContainerInfo>> containers; 
        static std::mutex containersMutex;

        std::shared_ptr<ContainerInfo> findContainer(const std::string& id);
        bool startContainer(const std::string& id);
        bool stopContainer(const std::string& id);
        std::string getContainerStatus(const std::string& id);
        ~ContainerService() = default;
        std::shared_ptr<ContainerInfo> loadConfigJson(const Job& J);

};


std::shared_ptr<ContainerInfo> ContainerService::loadConfigJson(const Job& job_ref) {
    try {
        std::string filePath = job_ref.location + "/cassie-config.json";
        std::ifstream file(filePath);
        if(!file.is_open()){
            throw std::runtime_error("[-] loadConfigJson failed");
        }
        json j;
        try {
            file >> j;
        }   catch (json::parse_error& e){
            std::string error = "[-] failed to parse cassie-config.json in loadConfigJson for ContainerService";
            throw std::runtime_error(error);
        }
        auto newContainer = std::make_shared<ContainerInfo>();
        {
            j.at("baseImage").get_to(newContainer->Image);
            if (j.contains("workingDirectory")) j.at("workingDirectory").get_to(newContainer->workingDir);
            if (j.contains("copyPaths")) j.at("copyPaths").get_to(newContainer->copyPaths);
            if (j.contains("aptPackages")) j.at("aptPackages").get_to(newContainer->aptPkgs);
            if (j.contains("buildCommands")) j.at("buildCommands").get_to(newContainer->b_commands);
            if (j.contains("entrypoint")) j.at("entrypoint").get_to(newContainer->entryPoint);
            if (j.contains("cmd")) j.at("cmd").get_to(newContainer->cmd);
            if (j.contains("exposedPorts")) j.at("exposedPorts").get_to(newContainer->exposedPorts);
            if (j.contains("envVars")) j.at("envVars").get_to(newContainer->envVars);
            if (j.contains("volumes")) j.at("volumes").get_to(newContainer->volumes);
        }
        return newContainer;
    } catch (std::runtime_error& e){
        std::cout << "[ContainerService] LoadConfigJson Failed :" << e.what(); 
    }
    
}

std::shared_ptr<ContainerInfo> ContainerService::findContainer(const std::string& id) {
    if(id == "container_null"){
        return nullptr;
    }
    std::unique_lock lock(containersMutex);
    auto it = std::find_if(containers.begin(), containers.end(), [&](const auto& container) {
        return container->id == id;
    });
    return it != containers.end()? *it : nullptr;
}

bool ContainerService::startContainer(const std::string& id) {
    std::shared_lock lock(containersMutex);
    auto container = findContainer(id);
    std::shared_lock unlock(containersMutex);
    httplib::Client cli("/var/run/docker.sock/v1.41/");

    //auto res = fetch(cli, "")
}
