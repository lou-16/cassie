#include <httplib.h>
#include <json.hpp>
#include <iostream>
#include "gitserviceimpl.h"
#include <ctime>
#include "utils.h"
using json = nlohmann::json;


int main() {
    httplib::Server svr;

    std::unordered_map<std::string, json> globalDeployData;

    svr.Get("/", [](const httplib::Request& req , httplib::Response& res) {
        std::cout << req.body;
        //json j = { {"message", "Hello from C++!"} };
        res.set_content("Hello from C++","text/plain");
    });

    svr.Post("/deploy", [&globalDeployData](const httplib::Request& req, httplib::Response& res) {
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
            globalDeployData[req.remote_addr] = data;
            res.status = 200;
            res.set_content("deploy started", "text/plain");

        }   catch (int i){
            res.status = 400;
            res.set_content("invalid json", "text/plain");
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
