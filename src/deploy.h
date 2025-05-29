/*
    DEPLOY.H file
    purpose: handling deploying a git repository using a containerization software.
*/

#pragma once
#include <string>

class DeploymentService {
public:
    virtual ~DeploymentService() = default;

    // Main method to deploy the application from a local directory
    virtual bool deploy(const std::string& repoPath) = 0;

    // Optionally return logs or status
    virtual std::string getLastLog() const = 0;
};
