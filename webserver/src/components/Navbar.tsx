import { useEffect, useState } from "react";
import { useLocation, useNavigate } from "react-router-dom"
import { auth } from "../firebase";
import { onAuthStateChanged, type User } from "firebase/auth";

export default function Navbar(){

    const [currentUser, setCurrentUser] = useState<User | null>(null);
    const [IsLoginPage, setLoginPage] = useState<boolean> (false)
    const loc = useLocation();
    const navigator = useNavigate();
    useEffect(()=> {
        const getIfUserLoggedIn = ()=> {
            onAuthStateChanged(auth, (currentUser) => {
                setCurrentUser(currentUser);
            }) 
        }
        const getIfCurrentActivePageIsLogin = () => {
            if(loc.pathname == "/Login") setLoginPage(true)
            setLoginPage(false)
        }
        getIfUserLoggedIn();
        getIfCurrentActivePageIsLogin();
    }, [])



    return (
    <div className="flex items-center px-6 py-4 w-full bg-[#010101] text-white justify-between min-h-20">
        <div className="flex flex-row">
        <a href="/">
            <h1 className="text-3xl pl-8 pr-8 font-bold tracking-wide">Cassie</h1>
        </a>  
        { !currentUser &&
            <nav className="flex gap-6 pl-16">
                <a href="features" className="pl-8 pr-4 hover:text-glow transition-all duration-150 hover:text-gray-400 text-xl ">Features</a>
                <a href="cli" className="pl-4 pr-4 hover:text-glow transition-all duration-150 hover:text-gray-400 text-xl ">CLI</a>
                <a href="github" className="pl-4 pr-8 hover:text-glow transition-all duration-150 hover:text-gray-400 text-xl">GitHub</a>
            </nav>
        }
        {
            
        }
        </div>
        
        { (!currentUser && !IsLoginPage) &&
            <button onClick={() => navigator("/Login")}className="mr-16 p-2 pr-6 pl-6 rounded-md border hover:bg-white hover:text-black transition-all duration-150 ease-in-out">Login</button>
        } 
        { IsLoginPage 
        }
            <button onClick={() => navigator("/Login")}className="hidden mr-16 p-2 pr-6 pl-6 rounded-md border hover:bg-white hover:text-black transition-all duration-150 ease-in-out">Login</button>
        {
            currentUser && 
            <div>
                <span>Welcome, {currentUser.email} 
                <button onClick={() => navigator("/Logout")} className="mr-16 ml-16 pl-4 pr-4 p-1 rounded-md border hover:bg-white hover:text-black transition-all duration-150 ease-in-out">Logout</button>
                </span>
            </div>
        }
</div>)
}