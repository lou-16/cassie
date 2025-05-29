#include "gitserviceimpl.h"
#include <iostream>
#include <cstdlib>

bool GitServiceImpl::handleRequest(const std::string& repoURL, const std::string& pathdir){
    std::cout << "Pulling Git Repo from URL" << repoURL << std::endl;\
    std::string command = "git clone " + repoURL + " " + pathdir;
    return std::system(command.c_str()) == 0;
}