import CpuChart from "../components/CpuChart";
import ProjectCard from "../components/ProjectCard";
import DeploymentPanel from "../components/DeploymentPanel";
import React, { useEffect } from "react";

const dummyProjects = [
  {
    id: 1,
    name: "Cassie Frontend",
    status: "Active",
    lastDeployed: "2 min ago",
    cpuUsage: 63,
  },
  {
    id: 2,
    name: "User Auth Service",
    status: "Idle",
    lastDeployed: "1 hour ago",
    cpuUsage: 29,
  },
  {
    id: 3,
    name: "Static Assets",
    status: "Active",
    lastDeployed: "Just now",
    cpuUsage: 91,
  },
];
export default function Dashboard() {

  const [projects, setProjects] = React.useState(dummyProjects);
        useEffect(() => {
        const interval = setInterval(() => {
        setProjects((prev) =>
            prev.map((p) => ({
            ...p,
            cpuUsage: Math.floor(Math.random() * 100),
            }))
        );
        }, 5000); // refresh every 5s

        return () => clearInterval(interval);
        }, []);
  return (
    <div className="min-h-screen bg-zinc-950 text-white p-6">
      <h1 className="text-3xl font-bold mb-6">Dashboard</h1>

      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Left side: CPU + deployment */}
        <div className="lg:col-span-2 flex flex-col gap-6">
          <div className="bg-zinc-900 rounded-xl p-4 shadow-lg h-64">
            <h2 className="text-xl font-semibold mb-2">CPU Usage</h2>
            <CpuChart />
          </div>

          <div className="bg-zinc-900 rounded-xl p-4 shadow-lg h-64">
            <h2 className="text-xl font-semibold mb-2">Live Deployment Info</h2>
            <DeploymentPanel />
          </div>
        </div>

        {/* Right side: Active Projects */}
        <div className="bg-zinc-900 rounded-xl p-4 shadow-lg">
          <h2 className="text-xl font-semibold mb-4">Active Projects</h2>
          <div className="flex flex-col gap-4">
            {projects.map((project) => (
              <ProjectCard key={project.id} project={project} />
            ))}
          </div>
        </div>
      </div>
    </div>
  );
}
