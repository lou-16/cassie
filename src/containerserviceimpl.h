#include "containerservice.h"

class ContainerServiceImpl : public ContainerService {
public:
    bool startContainer(const std::string& imageName, const std::string& containerName) override {
        std::string cmd = "docker run -d --name " + containerName + " " + imageName;
        return std::system(cmd.c_str()) == 0;
    }

    bool stopContainer(const std::string& containerName) override {
        std::string cmd = "docker stop " + containerName + " && docker rm " + containerName;
        return std::system(cmd.c_str()) == 0;
    }

    std::string getContainerStatus(const std::string& containerName) override {
        std::string cmd = "docker inspect -f '{{.State.Status}}' " + containerName;
        std::string output;
        FILE* pipe = _popen(cmd.c_str(), "r");
        if (!pipe) return "error";
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        _pclose(pipe);
        return output;
    }
};
