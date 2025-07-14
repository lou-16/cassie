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

function App() {

  return (
    <div>
      
      <div className="w-full h-full fixed top-0 left-0">
        <ThreeCanvas />
      </div>
    <div className="relative z-10 flex flex-col min-h-screen">
      <Navbar />
    <Routes>   
      <Route path="/" element={<Home />} />
      <Route path="/Login" element={<LoginPage />} />
      <Route path="/Dashboard" element={<ProtectedRoute> <Dashboard /> </ProtectedRoute>} />
      <Route path="*" element={<NotFound />} />
    </Routes>
    <Footer />
    </div>
    <Toaster />
    </div>
  )
}

export default App
