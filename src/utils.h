#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>

std::string appendEpochTo(const std::string& base);
uint64_t cheapHash(uint64_t timestamp, uint64_t salt);
std::string toHex(uint64_t value);

std::string createUniqueId();
std::string extractRepoName(const std::string& url);

FetchResponse fetch(
    httplib::Client& client, 
    const std::string& endpoint, 
    const std::string& method = "GET",
    const httplib::Headers& headers = {},
    const std::string& body = "",
    const std::string& content_type = "application/json"
); 