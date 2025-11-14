import ProjectCard from "../components/ProjectCard";
import Searchbar from '../components/Searchbar'

import React, { useEffect } from "react";
import { ProtectedRoute } from "../components/ProtectedRoute";

const BACKEND_URL = "http://localhost:3000/";

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
export default function Dashboard() {

  return (
    <ProtectedRoute>
      <div>
      <Searchbar className="w-3xl" />

      </div>
    </ProtectedRoute>
  );
}
