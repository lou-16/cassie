// src/layouts/BaseLayout.tsx
import Navbar from "../components/Navbar";

export default function BaseLayout({ children }: { children: React.ReactNode }) {
  return (
    <div className="flex flex-col min-h-screen bg-black text-white">
      <Navbar />
      <main className="flex-1">{children}</main>
    </div>
  );
}
