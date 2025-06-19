#include "utils.h"

std::string appendEpochTo(const std::string& base){
    using namespace std::chrono;
    auto now = system_clock::now();
    auto epoch = duration_cast<seconds>(now.time_since_epoch()).count();
    return base + "_" + std::to_string(epoch);
}

uint64_t cheapHash(uint64_t timestamp, uint64_t salt){
    return (timestamp ^ (salt << 13) )  ^(timestamp >> 7) ^ salt;
}

std::string toHex(uint64_t value){
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << value;
    return oss.str();
}

std::string createUniqueId(){
    using namespace std::chrono;
    uint64_t timestamp = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();

    std::random_device rd;
    std::mt19937_64 gen(rd());
    uint64_t salt = gen();

    uint64_t hashed = cheapHash(timestamp, salt);

    std::string id = toHex(timestamp)+toHex(salt).substr(0, 4)+ toHex(hashed).substr(0, 0);
    return id;
}