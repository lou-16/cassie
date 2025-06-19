#include <httplib.h>
#include <json.hpp>
#include <iostream>
#include "gitserviceimpl.h"
#include <ctime>
#include "utils.h"
#include "buildserviceimpl.h"
using json = nlohmann::json;

BuildServiceImpl build_service;

int main() {
    httplib::Server svr;
    
    

    svr.Get("/", [](const httplib::Request& req , httplib::Response& res) {
        std::cout << req.body;
        //json j = { {"message", "Hello from C++!"} };
        res.set_content("Hello from C++","text/plain");
    });

    svr.Post("/deploy", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json data = json::parse(req.body);
            const auto tp_utc{std::chrono::system_clock::now().time_since_epoch()};
            std::string repoURL = data["repo"];
            std::string targetDir = appendEpochTo("../deployments/kernel");
            
            GitServiceImpl s;
            if(!s.handleRequest(repoURL, targetDir)){
                res.status = 501;
                res.set_content("deploy failed", "text/plain");
                return;    
            }
            std::string job_id = createUniqueId();
            std::thread([repoURL, targetDir, job_id](){
                try  {
                    JOB_STATUS __status = build_service.addJobToQueue(job_id, targetDir, BUILD_TYPE::CPP);
                    build_service.executeJob(job_id);
                } catch (const std::exception& e){
                    std::cerr<< "[THREAD ERROR]" << e.what() << std::endl;
                }
            }).detach();

            json responseData;
            responseData["message"] = "your deployment request has been pushed for processing";
            responseData["id"] = job_id;
            res.status = 200;
            res.set_content(responseData.dump(), "application/json");

        }   catch (const std::exception& i){
            res.status = 400;
            res.set_content(std::string("invalid json") + i.what() , "text/plain");
        }
        
    });

    svr.Get("/status", [](const httplib::Request& req, httplib::Response& res){

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
