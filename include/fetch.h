#ifndef FETCH_H
#define FETCH_H

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
) ;
#endif