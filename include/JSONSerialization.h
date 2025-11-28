#pragma once
#include "json.hpp"

struct ContainerInfo;
struct Job;

using json = nlohmann::json;

// container
void to_json(json& j, const ContainerInfo& c);
void from_json(const json& j, ContainerInfo& c);

// job
namespace ns {
    void to_json(json& j, const Job& job);
    void from_json(const json& j, Job& job);
}
