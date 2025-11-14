// src/components/SubNavbar.tsx
export default function SubNavbar() {
  const tabs = [
    "Overview",
    "Integrations",
    "Deployments",
    "Activity",
    "Domains",
    "Usage",
    "Settings",
  ];

  return (
    <div className="flex gap-6 px-8 py-3 bg-[#0d0d0d] text-gray-300 border-b border-gray-800 overflow-x-auto self-center border-t">
      {tabs.map((tab) => (
        <button
          key={tab}
          className="hover:text-white transition-colors duration-150 whitespace-nowrap"
        >
          {tab}
        </button>
      ))}
    </div>
  );
}

