/**
 * Git Service:
 * purpose: handling git related tasks.
 */

#pragma once 
#include <string>

class GitService {
    public:
        virtual ~GitService() = default;

        virtual bool handleRequest(const std::string& repoURL, const std::string& targetDir);
};