import ProjectCard from "../../components/ProjectCard";
import { ProtectedRoute } from "../../components/ProtectedRoute";

const usageStats = [
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

export default function Overview() {
      return (
          <div className="min-h-screen bg-zinc-950 text-white p-6">
            <h1 className="text-3xl font-bold mb-6">Usage Stats (placeholder)</h1>
    
            <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 items-center">
              <div className="bg-zinc-900 rounded-xl p-4 shadow-lg self-center">
                <h2 className="text-xl font-semibold mb-4">Active Projects</h2>
                <div className="flex flex-col gap-4">
                  {usageStats.map((project) => (
                    <ProjectCard key={project.id} project={project} />
                  ))}
                </div>
              
              </div>
              <div className="bg-zinc-900 rounded-xl p-4 shadow-lg self-center col-span-2">
                  
              </div>
            </div>
          </div>
      );
}