interface Project {
  id: number;
  name: string;
  status: string;
  lastDeployed: string;
  cpuUsage: number; // new field
}

export default function ProjectCard({ project }: { project: Project }) {
  const statusColor =
    project.status === "Active" ? "text-green-400" : "text-yellow-400";

  const cpuColor =
    project.cpuUsage > 80
      ? "bg-red-500"
      : project.cpuUsage > 50
      ? "bg-yellow-500"
      : "bg-green-500";

  return (
    <div className="bg-zinc-800 p-4 rounded-lg shadow-sm space-y-2">
      <div className="flex justify-between items-center">
        <h3 className="font-medium">{project.name}</h3>
        <span className={`text-sm ${statusColor}`}>{project.status}</span>
      </div>

      <p className="text-sm text-gray-400">Last deployed: {project.lastDeployed}</p>

      <div className="text-sm text-gray-300">CPU Usage: {project.cpuUsage}%</div>
      <div className="w-full h-2 bg-zinc-700 rounded-full overflow-hidden">
        <div
          className={`h-full ${cpuColor}`}
          style={{ width: `${project.cpuUsage}%` }}
        />
      </div>
    </div>
  );
}
