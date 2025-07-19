import { useNavigate } from "react-router-dom"

export default function Navbar(){

    const navigator = useNavigate();
    return (
    <div className="flex items-center px-6 py-4 w-full bg-[#010101] text-white justify-between min-h-20">
        <div className="flex flex-row">
        <a href="/">
            <h1 className="text-3xl pl-8 pr-8 font-bold tracking-wide">Cassie</h1>
        </a>  
            <nav className="flex gap-6 pl-16">
            <a href="features" className="pl-8 pr-4 hover:text-glow transition-all duration-150 hover:text-gray-400 text-xl ">Features</a>
            <a href="cli" className="pl-4 pr-4 hover:text-glow transition-all duration-150 hover:text-gray-400 text-xl ">CLI</a>
            <a href="github" className="pl-4 pr-8 hover:text-glow transition-all duration-150 hover:text-gray-400 text-xl">GitHub</a>
        </nav>
        </div>
        <button onClick={() => navigator("/Login")}className="mr-16 p-2 pr-6 pl-6 rounded-md border hover:bg-white hover:text-black transition-all duration-150 ease-in-out">Login</button>
    </div>)
}