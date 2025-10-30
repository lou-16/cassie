#ifndef CONTAINERSERVICE_H
#define CONTAINERSERVICE_H
#include <thread>
#include <fstream>
#include <memory>
#include <streambuf>
#include <unistd.h>

#include "httplib.h"
#include "json.hpp"
#include "ContainerQueries.h"
#include "fetch.h"
#include "utils.h"
#include "schedulerserviceimpl.h"
#include <optional>



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
            if(client.is_valid()){
                std::cout << "connected to docker socket\n";
            }
        };
        /* key : jobId from Scheduler, key : unique_ptr to the container associated  */
        static std::unordered_map<std::string,std::unique_ptr<ContainerInfo>> containersMap;
        static std::shared_mutex containersMutex; /* mutex for the containersMap */
     
        /* class methods / API for communication with the Docker UNIX socket */
        
        /* non-const returned, they can change the inside of the function*/
        
        /* API */
        std::optional<std::reference_wrapper<ContainerInfo>> findContainer (const std::string& JobID); //
        bool startContainer                         (const std::string& JobID); //
        bool stopContainer                          (const std::string& JobID); //
        std::string getContainerStatus              (const std::string& JobID); //
        std::optional<std::reference_wrapper<ContainerInfo>> createContainer(const std::string& JobID);

        /* destructor TODO : delete/free all ptrs to the containers. this will cause a memory leak if not fixed */
        ~ContainerService() = default; // 

        /* this adds the unique ptr to container to the map TODO: design this better later */
        void initialiseContainerInfo(Job& J, const json& config); // 

        /* TODO (IMPORTANT) : extend this class to allow for TCP streams to be sent to the backend */
};

#endif /* CONTAINERSERVICE_H*/