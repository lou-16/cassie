#include <httplib.h>
#include <json.hpp>
#include <iostream>
#include "git/gitserviceimpl.h"
#include <ctime>
#include "utils/utils.h"
#include "scheduler/schedulerserviceimpl.h"
#include "Docker/containerservice.h"

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


    // the body should be of this format:
    /*
        {
            "repo" : "{GIT_REPO_URL_PUBLIC}",
        }
    */
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

svr.Get("/containers/:id/status", [&](const httplib::Request& req, httplib::Response& res){
    try {
        json data = json::parse(req.body);
        auto user_id = req.path_params.at("id");
        std::string containerStatus = ContainerServiceObject.getContainerStatus(user_id);
        if(user_id == ""){
            throw new std::exception();
        }
        else {
            json jsonResponse = 
            {
                {"status" , std::string(containerStatus)}
            };
            res.status = httplib::OK_200;
            res.set_content(jsonResponse.dump(), "application/json");
        }
    } catch (const std::exception& e){
        res.status = httplib::NotFound_404;
        res.set_content("failure", "text/plain");
    }
});

svr.Post("/containers/:id/create", [&](const httplib::Request& req, httplib::Response& res) {
    try {
        json data = json::parse(req.body);
        auto container_id = req.path_params.at("id");
        std::string job_id = data["JobID"].is_string() ? data["JobID"] : "";
        Job& jobRef = Scheduler.getJobRef(job_id);
        if(jobRef.__id == emptyJob.__id){
            res.status = httplib::BadRequest_400;
            res.set_content("jobId in the post req body does not correspond to a correct container/ job", "text/plain");
        }
        std::string id = ContainerServiceObject.createContainer(jobRef);
        json response = 
        {
            {"status", 200},
        };
        
        res.status = httplib::OK_200;
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = httplib::Forbidden_403;
        res.set_content("forbidden", "text/plain");
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
