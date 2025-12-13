#include "JSONSerialization.h"
#include "containerservice.h"
#include "schedulerservice.h"

// ------------------ ContainerInfo ------------------------

void to_json(json& j, const ContainerInfo& c)
{
    j = {
        {"status",        c.status},
        {"id",            c.id},
        {"jobId",         c.jobId},
        {"Image",         c.Image},
        {"workingDir",    c.workingDir},
        {"copyPaths",     c.copyPaths},
        {"aptPkgs",       c.aptPkgs},
        {"buildCommands", c.buildCommands},
        {"entryPoint",    c.entryPoint},
        {"cmd",           c.cmd},
        {"exposedPorts",  c.exposedPorts},
        {"envVars",       c.envVars},
        {"volumes",       c.volumes}
    };
}

void from_json(const json& j, ContainerInfo& c)
{
    j.at("status").get_to(c.status);
    j.at("id").get_to(c.id);
    j.at("jobId").get_to(c.jobId);
    j.at("Image").get_to(c.Image);
    j.at("workingDir").get_to(c.workingDir);

    j.at("copyPaths").get_to(c.copyPaths);
    j.at("aptPkgs").get_to(c.aptPkgs);
    j.at("buildCommands").get_to(c.buildCommands);

    j.at("entryPoint").get_to(c.entryPoint);
    j.at("cmd").get_to(c.cmd);
    j.at("exposedPorts").get_to(c.exposedPorts);
    j.at("envVars").get_to(c.envVars);
    j.at("volumes").get_to(c.volumes);
}



// -------------------------- Job --------------------------

namespace ns {

void to_json(json& j, const Job& job)
{
    std::vector<std::string> containerIds;
    containerIds.reserve(job.__containers.size());

    for (const auto& ref : job.__containers)
        containerIds.emplace_back(ref);

    j = {
        {"jobId",      job.__id},
        {"config",     job.__config},
        {"build_type", job.__build_type},
        {"containers", containerIds}
    };
}

void from_json(const json& j, Job& job)
{
    j.at("jobId").get_to(job.__id);
    j.at("config").get_to(job.__config);
    j.at("build_type").get_to(job.__build_type);

    // store IDs only — actual container binding done later
    j.at("containers").get_to(job.__containerIds);
}

} // namespace ns


namespace ns 
{
    inline void to_json(nlohmann::json& j, const createContainerQuery& c)
    {
        j = nlohmann::json{
            {"Hostname" , c.Hostname},
            {"Domainame", c.Domainame},
            {"User", c.User},
            {"AttachStdin", c.AttachStdin},
            {"AttachStdOut", c.AttachStdOut},
            {"ExposedPorts", c.ExposedPorts},
            {"Tty", c.Tty},
            {"OpenStdin", c.OpenStdin},
            {"StdinOnce", c.StdinOnce},
            {"Env", c.Env},
            {"Cmd", c.Cmd},
            {"Healthcheck", c.Healthcheck},
            {"ArgsEscaped", c.ArgsEscaped || false},
            {"Image" , c.Image},
            {"Volumes", c.Volumes},
            {"WorkingDir", c.WorkingDir},
            {"Entrypoint", c.Entrypoint},
            {"NetworkDisabled", c.NetworkDisabled},
            {"MacAddress", c.MacAddress},
            {"OnBuild", c.Onbuild},
            {"Labels", c.Labels},
            {"StopSignal", c.StopSignal},
            {"StopTimeout", c.Stoptimeout},
            {"Shell", c.shell},
            {"HostConfig", c.HostConfig},
            {"NetworkingConfig", c.NetworkingConfig}
        };
    }

    

}