// src/layouts/DashboardLayout.tsx
import BaseLayout from "./BaseLayout.tsx";
import SubNavbar from "../components/SubNavbar";

export default function DashboardLayout({ children }: { children: React.ReactNode }) {
  return (
    <BaseLayout>
      <SubNavbar />
      <div className="p-8">{children}</div>
    </BaseLayout>
  );
}
