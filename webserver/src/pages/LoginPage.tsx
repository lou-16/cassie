import { useNavigate } from "react-router-dom";
import Footer from "../components/Footer";
import LoginModal from "../components/LoginModal";
import { useEffect } from "react";
import { onAuthStateChanged } from "firebase/auth";
import { auth } from "../firebase";


const LoginPage = () => {
    const navigate = useNavigate()
    useEffect(()=> {
        const unsubscribe = onAuthStateChanged(auth,(user)=> {
            if(user) {
                navigate("/dashboard");
            }
        });
        return () => unsubscribe();
    }, [navigate]);
    return(<div className="min-h-[700px] h-fit  bg-transparent">
        <LoginModal />
        <Footer />
    </div>)
}

export default LoginPage;