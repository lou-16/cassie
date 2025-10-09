import {Routes, Route} from "react-router-dom"
import Home from "./pages/Home"
import LoginPage from "./pages/LoginPage"
import Navbar from "./components/Navbar"
import Footer from "./components/Footer"
import NotFound from "./pages/NotFound"
import ThreeCanvas from "./components/ThreeCanvas"
import { Toaster } from "react-hot-toast"
import { ProtectedRoute } from "./components/ProtectedRoute"
import Dashboard from "./pages/Dashboard"
import { RotateNode } from "three/webgpu"
import SignupPage from "./pages/SignupPage"
import DashboardLayout from "./layouts/DashboardLayout"
import BaseLayout from "./layouts/BaseLayout"
import Overview from "./pages/Dashboard/Overview"
import Deployments from "./pages/Dashboard/Deployments"
import Domains from "./pages/Dashboard/Domains"
import Settings from "./pages/Dashboard/Settings"

function App() {

  return (
    <div>
      
      <div className="w-full h-full fixed top-0 left-0">
        <ThreeCanvas />
      </div>
    <div className="relative z-10 flex flex-col min-h-screen">
    <Routes>   
      <Route path="/" element={ <BaseLayout><Home /></BaseLayout>} />
      <Route path="/Login" element={<BaseLayout><LoginPage /></BaseLayout>} />
      <Route path="/Dashboard" element={
        <DashboardLayout ><Dashboard /> </DashboardLayout>}>
          <Route index element={<Overview/>} />
          <Route path="Deployments" element={<Deployments />} />
          <Route path="Domains" element={<Domains />} />
          <Route path="Settings" element={<Settings />} />
      </Route>
      <Route path="*" element={<BaseLayout><NotFound /></BaseLayout>} />
      <Route path="/SignUp" element={<BaseLayout> <SignupPage /></BaseLayout>} />
    </Routes>
    <Footer />
    </div>
    <Toaster />
    </div>
  )
}

export default App
