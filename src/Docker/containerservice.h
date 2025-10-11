#pragma once 

#include <thread>
#include <fstream>
#include <memory>
#include <streambuf>
#include <unistd.h>

#include "httplib.h"
#include "json.hpp"
#include "ContainerQueries.h"
#include "../utils/fetch.h"
#include "../utils/utils.h"
#include "../scheduler/schedulerserviceimpl.h"



using json = nlohmann::json;

class ContainerService {
    private:
        httplib::Client client;
    public:
        ContainerService() : client("unix://var/run/docker.sock") {
            client.set_connection_timeout(5); //seconds
            client.set_read_timeout(5);
            client.set_write_timeout(5);
        };
        static std::vector<std::shared_ptr<ContainerInfo>> containers; 
        static std::unordered_map<std::string,std::shared_ptr<ContainerInfo>> containersMap;
        static std::mutex containersMutex;
        static std::condition_variable isAvailable;

        static std::vector< std::thread > stdioThreads;
        static const int max_size_threads = 10;
        static void stdio_function();

        std::shared_ptr<ContainerInfo> findContainer(const std::string& id); //
        bool startContainer(const std::string& name); //
        bool stopContainer(const std::string& name); //
        std::string getContainerStatus(const std::string& name); //
        ~ContainerService() = default; // 
        std::shared_ptr<ContainerInfo> intialiseContainerInfo(const Job& J); //
        std::string createContainer(const Job& J); //
        void initialiseThreads();
};

std::shared_ptr<ContainerInfo> ContainerService::intialiseContainerInfo(const Job& job_ref) {
    {
        std::unique_lock lck(containersMutex);
    }
    try {
        std::string filePath = job_ref.location + "/cassie-config.json";
        std::ifstream file(filePath);
        if(!file.is_open()){
            throw std::runtime_error("[-] intialiseContainerInfo failed");
        }
        json j;
        try {
            file >> j;
        }   catch (json::parse_error& e){
            std::string error = "[-] failed to parse cassie-config.json in intialiseContainerInfo for ContainerService";
            throw std::runtime_error(error);
        }
        auto newContainer = std::make_shared<ContainerInfo>();
        newContainer.get()->jobId = job_ref.__id;
        {
            j.at("baseImage").get_to(newContainer->Image);
            if (j.contains("workingDirectory")) j.at("workingDirectory").get_to(newContainer->workingDir);
            if (j.contains("copyPaths")) j.at("copyPaths").get_to(newContainer->copyPaths);
            if (j.contains("aptPackages")) j.at("aptPackages").get_to(newContainer->aptPkgs);
            if (j.contains("buildCommands")) j.at("buildCommands").get_to(newContainer->buildCommands);
            if (j.contains("entrypoint")) j.at("entrypoint").get_to(newContainer->entryPoint);
            if (j.contains("cmd")) j.at("cmd").get_to(newContainer->cmd);
            if (j.contains("exposedPorts")) j.at("exposedPorts").get_to(newContainer->exposedPorts);
            if (j.contains("envVars")) j.at("envVars").get_to(newContainer->envVars);
            if (j.contains("volumes")) j.at("volumes").get_to(newContainer->volumes);
        }
        containers.push_back(newContainer);
        containersMap[newContainer.get()->id] = newContainer;
        return newContainer;
    } catch (std::runtime_error& e){
        std::cout << "[ContainerService] intialiseContainerInfo Failed :" << e.what(); 
    }
    
}

std::shared_ptr<ContainerInfo> ContainerService::findContainer(const std::string& id) {
    if(id == ""){
        return nullptr;
    }
    std::unique_lock lock(containersMutex);
    auto it = containersMap.find(id);
    return (it != containersMap.end()) ? it->second : nullptr;
}

bool ContainerService::startContainer(const std::string& name) {
    
    std::shared_ptr<ContainerInfo> container = findContainer(name);
    if(!container) {
        return false;
    }

    if(container.get()-> id == ""){
        return false;
    }
    std::string endpoint = "/1.41/containers/" + container.get()->id + "/start";

    FetchResponse res = fetch(client, endpoint, "POST");

    return res.status == 204;
}

std::string ContainerService::createContainer(const Job& j) {

    

    std::shared_ptr<ContainerInfo> Container  = intialiseContainerInfo(j);
    
    std::string endpoint = "/1.41/containers/create";

    json body = {
        //implement proper image support, for now, just get hello-world working
        {"Image" , "hello-world"}
    };


    httplib::Headers headers = {
        {"Content-Type" , "application/json"}
    };

    FetchResponse res = fetch(client, endpoint, "POST", headers, body.dump());

    {
        std::unique_lock lock(containersMutex);
        isAvailable.notify_one();
    }

    json responseToBeParsed = json::parse(res.body, nullptr, false);
    if(res.status == 201 && responseToBeParsed.contains("id")){
        Container.get()->id = responseToBeParsed["id"];
    } else if (responseToBeParsed.contains("message")){
        Container.get()->id = "";

        return "";
    }
    else{
        return "";
    }
    return Container.get()->id;
    
}

bool ContainerService::stopContainer(const std::string& id) {
    std::shared_lock lock(containersMutex);
    std::shared_ptr<ContainerInfo> container = findContainer(id);
    if (!container) {
        return false;
    }
    if(container.get()->id == ""){
        return false;
    }
    std::string endpoint = "/1.41/containers/" + container.get()->id + "/kill";

    FetchResponse res = fetch(client, endpoint, "POST");

    return res.status == 204;
}

std::string ContainerService::getContainerStatus(const std::string& id){
    std::shared_lock lock(containersMutex);
    std::shared_ptr<ContainerInfo> container = findContainer(id);
    if(!container) {
        return "CONTAINER_NOT_FOUND";
    }
    if(container.get()->id == ""){
        return "CONTAINER_NOT_INIT";
    }
    std::string endpoint = "/1.41/containers/" + container.get()->id + "/json";

    FetchResponse res = fetch(client, endpoint, "GET");

    if(res.status == 200) {
        try {
            std::string state = json(res.body)["State"]["Status"];
            return state;
        } catch (const std::exception& e){
            return "PARSE_ERROR";
        }
    }
    return "FETCH_FAILED";

}

void ContainerService::initialiseThreads() {
    for(int i = 0; i < max_size_threads; i++) {
        stdioThreads[i] = std::thread(stdio_function);
    }
    return;
}

void ContainerService::stdio_function() {
    while(true) {
        std::unique_lock lck(containersMutex);
        isAvailable.wait(lck, [](){return !ContainerService.})
    }
}
