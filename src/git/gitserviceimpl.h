#pragma once 
#include "gitservice.h"

class GitServiceImpl : public GitService {
    public:
        bool handleRequest(const std::string& repoURL, const std::string& pathDir);
};