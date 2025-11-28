#include "containerservice.h"

std::unordered_map<std::string,std::unique_ptr<ContainerInfo>> ContainerService::containersMap;
std::shared_mutex ContainerService::containersMutex;

void ContainerService::initialiseContainerInfo(Job& job_ref, const json& config) {
    {
        // lock the mutex for exclusive access
        std::unique_lock lck(containersMutex);
    }
    try {
         
        auto newContainer = std::make_unique<ContainerInfo>();
        newContainer.get()->jobId = job_ref.__id;
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
        containersMap[newContainer.get()->id] = std::move(newContainer);
        return ;
    } catch (std::runtime_error& e){
        std::cout << "[ContainerService] intialiseContainerInfo Failed :" << e.what(); 
        return  ;
    }
}

ContainerInfo* ContainerService::findContainer(const std::string& id) {
    if(id == ""){
        return nullptr;
    }
    std::unique_lock lock(containersMutex);
    auto it = containersMap.find(id);
    if(it == containersMap.end()){
        return nullptr;
    }
    return (it->second.get());
}

bool ContainerService::startContainer(const std::string& name) {
    
    auto container = findContainer(name);
    if(container == nullptr) {
        return false;
    }

    std::string endpoint = "/1.41/containers/" + container->id + "/start";

    FetchResponse res = fetch(client, endpoint, "POST");

    return res.status == 204;
}

/* before calling, verify whether the json passed has a valid docker body that can directly be put into the POST req*/
ContainerInfo* ContainerService::createContainer(const std::string& containerId) {

    ContainerInfo* Container = findContainer(containerId);
    if(!Container){
        std::cerr << "[ContainerService::createContainer] : failed to find container";
        return nullptr;
    }
    
    json body = nlohmann::json{{"Image" , Container->Image}};
    
    std::string endpoint = "/1.41/containers/create";;
    httplib::Headers headers = {
        {"Content-Type" , "application/json"}
    };
    
    FetchResponse res = fetch(client, endpoint, "POST", headers, /*[+]*/body.dump());
    
    json responseToBeParsed = json::parse(res.body, nullptr, false);

    if(res.status == 201 && responseToBeParsed.contains("id")){
        Container->id = responseToBeParsed["id"];
        return Container;
    } 
    /* if response from the socket contains a message */
    else if (responseToBeParsed.contains("message")){
        /* set id to empty str */
        Container->id = "";
        /* return an error that can be straight up sent to the backend */
        return nullptr;
    }
    else{
        /* if anything wack happens */
        return nullptr;
    }
    return nullptr;
}

bool ContainerService::stopContainer(const std::string& containerId) {
    /* find the pointer to the container */
    auto container = findContainer(containerId);
    
    /* if null pointer */
    if (!container) {
        return false;
    }
    /* if createContainer failed, this container should not exist, but just a sanity check*/
    if(container->id== ""){
        /* if such a container is found, we simply remove it from map, stop it from polluting it */
        {
            std::scoped_lock lock(containersMutex);
            containersMap.erase(container->id);
        }
        /* call the destructor for this pointer */
        return false;
    }
    std::string endpoint = "/1.41/containers/" + container->id + "/kill";

    FetchResponse res = fetch(client, endpoint, "POST");

    return res.status == 204;
}

std::string ContainerService::getContainerStatus(const std::string& id){
    auto container = findContainer(id);
    if(!container) {
        return "CONTAINER_NOT_FOUND";
    }
    /* damn sometimes i overcheck dude. this SHOULD NOT EXIST. hmph */
    if(container->id == ""){
        {
            std::scoped_lock lock(containersMutex);
            containersMap.erase(container->jobId);
        }
        return "CONTAINER_NOT_INIT";
    }
    std::string endpoint = "/1.41/containers/" + container->id + "/json";

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

