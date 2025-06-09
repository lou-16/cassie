#include <chrono>
#include <string>
#include <iostream>

std::string appendEpochTo(const std::string& base){
    using namespace std::chrono;
    auto now = system_clock::now();
    auto epoch = duration_cast<seconds>(now.time_since_epoch()).count();
    return base + "_" + std::to_string(epoch);
}