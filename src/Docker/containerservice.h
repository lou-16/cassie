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
        /* TODO : Switch this out for a non-blocking socket I/O lib */
        ContainerService() : client("unix://var/run/docker.sock") {
            client.set_connection_timeout(5); //seconds
            client.set_read_timeout(5);
            client.set_write_timeout(5);
        };
        /* map that "maps" (lol) the jobID to the ContainerInfo struct.  */
        static std::unordered_map<std::string,std::shared_ptr<ContainerInfo>> containersMap;
        static std::mutex containersMutex; /* mutex for the containersMap */
        /* we lock the STL container due to multithreaded access being UB */

        /* class methods / API for communication with the Docker UNIX socket */
        std::shared_ptr<ContainerInfo> findContainer(const std::string& JobID); //
        bool startContainer(const std::string& JobID); //
        bool stopContainer(const std::string& JobID); //
        std::string getContainerStatus(const std::string& JobID); //
        ~ContainerService() = default; // 
        std::shared_ptr<ContainerInfo> initialiseContainerInfo(const std::shared_ptr<Job> J, const json& config); //
        std::string createContainer(const std::shared_ptr<Job> J); //

        /* TODO (IMPORTANT) : extend this class to allow for TCP streams to be sent to the backend */
};

std::shared_ptr<ContainerInfo> ContainerService::initialiseContainerInfo(std::shared_ptr<Job> job_ref, const json& config) {
    {
        // lock the mutex for exclusive access
        std::unique_lock lck(containersMutex);
    }
    try {
         
        auto newContainer = std::make_shared<ContainerInfo>();
        newContainer.get()->jobId = job_ref->__id;
        {
            config.at("baseImage").get_to(newContainer->Image);
            if (config.contains("workingDirectory")) config.at("workingDirectory").get_to(newContainer->workingDir);
            if (config.contains("copyPaths")) config.at("copyPaths").get_to(newContainer->copyPaths);
            if (config.contains("aptPackages")) config.at("aptPackages").get_to(newContainer->aptPkgs);
            if (config.contains("buildCommands")) config.at("buildCommands").get_to(newContainer->buildCommands);
            if (config.contains("entrypoint")) config.at("entrypoint").get_to(newContainer->entryPoint);
            if (config.contains("cmd")) config.at("cmd").get_to(newContainer->cmd);
            if (config.contains("exposedPorts")) config.at("exposedPorts").get_to(newContainer->exposedPorts);
            if (config.contains("envVars")) config.at("envVars").get_to(newContainer->envVars);
            if (config.contains("volumes")) config.at("volumes").get_to(newContainer->volumes);
        }
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

/* before calling, verify whether the job ptr passed has a valid docker body that can directly be put into the POST req*/
std::string ContainerService::createContainer(const std::shared_ptr<Job> j) {

    /* refs and endpoints along with POST req body and headers*/
    std::shared_ptr<ContainerInfo> Container  = containersMap[j.get()->__id];    
    std::string endpoint = "/1.41/containers/create";
    json body = j.get()->config;
    /* TODO : remove httplib dependency */
    httplib::Headers headers = {
        {"Content-Type" , "application/json"}
    };
    /* Fetch Response of POST request */
    FetchResponse res = fetch(client, endpoint, "POST", headers, body.dump());

    /* json that will stored info to be parsed. */
    json responseToBeParsed = json::parse(res.body, nullptr, false);

    /* if response is fine, then set container's id to docker container id*/
    if(res.status == 201 && responseToBeParsed.contains("id")){
        Container.get()->id = responseToBeParsed["id"];
    } 
    /* if response from the socket contains a message */
    else if (responseToBeParsed.contains("message")){
        /* set id to empty str */
        Container.get()->id = "";
        /* return an error that can be straight up sent to the backend */
        return "ERROR_" + responseToBeParsed["message"];
    }
    else{
        /* if anything wack happens */
        return "FAILURE";
    }
    /* return the container id, which can be stored in db/cache, linking to the jobID*/
    return Container.get()->id;
    
}

bool ContainerService::stopContainer(const std::string& containerId) {
    /* find the pointer to the container */
    std::shared_ptr<ContainerInfo> container = findContainer(containerId);
    
    /* if null pointer */
    if (!container) {
        return false;
    }
    /* if createContainer failed, this container should not exist, but just a sanity check*/
    if(container.get()->id == ""){
        /* if such a container is found, we simply remove it from map, stop it from polluting it */
        containersMap.erase(container.get()->jobId);
        /* call the destructor for this pointer */
        container.~shared_ptr();
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
    /* damn sometimes i overcheck dude. this SHOULD NOT EXIST. hmph */
    if(container.get()->id == ""){
        containersMap.erase(container.get()->jobId);
        container.~shared_ptr();
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


