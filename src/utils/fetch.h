#pragma once 

//simple header for fetch function.
#include "httplib.h"

struct FetchResponse {
    httplib::StatusCode status;
    std::string body;
};

FetchResponse fetch(
    httplib::Client& client, 
    const std::string& endpoint, 
    const std::string& method = "GET",
    const httplib::Headers& headers = {},
    const std::string& body = "",
    const std::string& content_type = "application/json"
) 
{
    std::shared_ptr<httplib::Result> res;
    
    if(method == "GET") {
        *res = client.Get(endpoint.c_str(),headers);
    } else if(method == "POST")
    {
        *res  = client.Post(endpoint.c_str(), headers, body, content_type.c_str());
    } else {
        std::cerr << "[Fetch failed] unsupported method, add the method in utils.cpp" << method << "\n";
        return {httplib::StatusCode::BadRequest_400, ""};
    }
    if (!res) {
        std::cerr << "[Fetch] No response from server" << std::endl;
        return {httplib::StatusCode::BadRequest_400, ""};
    }
    httplib::StatusCode status = httplib::StatusCode::OK_200;
    httplib::Response response = res->value();
    return {static_cast<httplib::StatusCode>(response.status), response.body};
}