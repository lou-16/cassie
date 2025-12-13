#include <httplib.h>
#include <json.hpp>
#include <iostream>
#include <ctime>
#include "utils.h"
#include "schedulerservice.h"
#include "containerservice.h"

using json = nlohmann::json;

int main()
{
    SchedulerService Scheduler;
    ContainerService ContainerServiceObject;
    httplib::Server svr;

    Scheduler.initWorkerPool();

    svr.Get("/", [](const httplib::Request &req, httplib::Response &res)
    {
        std::cout << req.body;
        //json j = { {"message", "Hello from C++!"} };
        res.set_content("cassie instance up!\n","text/plain"); 
    });

    
    /* POST /deployment/add
        {
            "repo": "repository_url",
            "config" : {},
            "project_type" : "nodejs"
        }

    */
    svr.Post("/deployments/add", [&](const httplib::Request &req, httplib::Response &res)
    {
    try {
        json data = json::parse(req.body);
        std::string repoURL = data["repo"];
        json config = data["config"]; // docker config
        json project_type = data["project_type"]; // nodejs
        try {
            std::string job_id = Scheduler.enqueueDeployment(repoURL, config, project_type);

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
        res.set_content(std::string("endpoint error: ") + e.what(), "text/plain");
    } });

    svr.Get("/deployments/containers/status", [&](const httplib::Request &req, httplib::Response &res)
            {
    try 
    {
        std::string user_id;
        if(req.has_param("id")){
            user_id = req.get_param_value("id");
        } else {
            throw new std::exception();
        }
        std::string containerStatus = ContainerServiceObject.getContainerStatus(user_id);
        
            json jsonResponse = 
            {
                {"status" , std::string(containerStatus)}
            };
            res.status = httplib::OK_200;
            res.set_content(jsonResponse.dump(), "application/json");
    } 
    catch (const std::exception& e)
    {
            res.status = httplib::NotFound_404;
            res.set_content("failure : " + std::string(e.what()), "text/plain");
    } });


    /*
        POST /deployment/containers/create
        {
            "id" : "id_string" (get it from the backend, db, i do not care. this should point to a valid jobID, given from prev req)
        }
    */
    svr.Post("/deployments/containers/create", [&](const httplib::Request &req, httplib::Response &res)
             {
    try {
        json data = json::parse(req.body);
        std::cerr << "[DEBUG] Parsed request body: " << data.dump(4) << "\n";

        if (!data.contains("id") || !data["id"].is_string()) {
            res.status = httplib::BadRequest_400;
            res.set_content("Invalid JSON: 'id' missing or not a string", "text/plain");
            return;
        }

        std::string job_id = data["id"];
        std::cerr << "[DEBUG] job_id = " << job_id << "\n";

        auto ref = Scheduler.getJobRef(job_id);
        std::cerr << "[DEBUG] does ref exist? and ref value is? : " << (ref.has_value()? "true" : "false") << "\n";
        if (!ref) {
            res.status = httplib::BadRequest_400;
            res.set_content("jobId in the post req body does not correspond to a correct container/job", "text/plain");
            return;
        }

        std::cerr << "[DEBUG] Creating container..." << "\n";
        ContainerServiceObject.initialiseContainerInfo(ref->get(), ref->get().__config);
        
        auto r = ContainerServiceObject.createContainer(ref->get().__id);

        std::cerr << "[DEBUG] Container created successfully." << "\n";
        // modern c++ is mad. how does this say that we r checking if a job's container set got a particular container or not?
        // who knows.
        if(ref->get().__containers.insert(r->id).second)
        {
            std::cerr << "[DEBUG] Container added to job's containers set" << "\n";
        }
        auto id = r->id;

        json response = {
            {"status", 200},
            {"containerId", id},
        };
        res.status = httplib::OK_200;
        res.set_content(response.dump(), "application/json");

    } catch (const nlohmann::json::exception& e) {
        std::cerr << "[JSON ERROR] " << e.what() << "\n";
        res.status = httplib::Forbidden_403;
        res.set_content("forbidden : " + std::string(e.what()), "text/plain");
    } catch (const std::exception& e) {
        std::cerr << "[STD ERROR] " << e.what() << "\n";
        res.status = httplib::Forbidden_403;
        res.set_content("forbidden : " + std::string(e.what()), "text/plain");
    } });

    /*
        GET /deployments/containers/logs?id="containerID"

        this will send the text logs that we have. we should see if stdout is to be supported. not supported for now for the web based interface.
        
    */
    svr.Get("/deployments/containers/logs", [&](const httplib::Request& req, httplib::Response& res){

    });

    svr.set_logger([](const httplib::Request &req, const httplib::Response &res)
                   { std::cout << "\n[LOG]" << req.method << " " << req.path << " -> " << res.status << " from " << req.remote_addr << "\n"; });

    std::cout << "\n[+]Server is up and running\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
