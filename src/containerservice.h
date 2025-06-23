#pragma once 

#include <thread>
#include "buildserviceimpl.h"

class ContainerService {
    public:
        virtual bool startContainer(const std::string& imageName, const std::string& containerName) = 0;
        virtual bool stopContainer(const std::string& containerName) = 0;
        virtual std::string getContainerStatus(const std::string& containerName) = 0;
        virtual ~ContainerService() = default;
};
