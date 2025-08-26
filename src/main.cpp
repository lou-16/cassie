#include <httplib.h>
#include <json.hpp>
#include <iostream>
#include "gitserviceimpl.h"
#include <ctime>
#include "utils.h"
#include "schedulerserviceimpl.h"
#include "containerservice.h"

using json = nlohmann::json;

SchedulerServiceImpl Scheduler;
ContainerService ContainerServiceObject;
int main() {
    httplib::Server svr;
    
    Scheduler.initWorkerPool();

    svr.Get("/", [](const httplib::Request& req , httplib::Response& res) {
        std::cout << req.body;
        //json j = { {"message", "Hello from C++!"} };
        res.set_content("Hello from C++","text/plain");
    });

    
svr.Post("/deploy", [](const httplib::Request& req, httplib::Response& res) {
    try {
        json data = json::parse(req.body);
        std::string repoURL = data["repo"];

        try {
            std::string outputFolder = "../deployments/" + extractRepoName(repoURL);
            std::string job_id = Scheduler.enqueueDeployment(repoURL, outputFolder);

            json responseData = {
                {"message", "your deployment request has been pushed for processing"},
                {"id", job_id}
            };
            res.status = 200;   
            res.set_content(responseData.dump(), "application/json");

        } catch (const std::exception& ex) {
            res.status = 501;
            res.set_content(std::string("deploy failed: ") + ex.what(), "text/plain");
        }

    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content(std::string("invalid json: ") + e.what(), "text/plain");
    }
});

svr.Post("/containers/:id", [&](const httplib::Request& req, httplib::Response& res){
    try {
        json data = json::parse(req.body);
        auto user_id = req.path_params.at("id");
        ContainerServiceObject.
    }
});

    svr.set_logger([](const httplib::Request &req, const httplib::Response &res){
        std::cout << "[LOG]"<< req.method << " " << req.path << " -> " << res.status << " from " << req.remote_addr << std::endl;
        if(req.method == "POST" && req.path == "/deploy"){
        }
    });

    std::cout << "Server is up and running";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
