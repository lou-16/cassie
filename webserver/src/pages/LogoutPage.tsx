import { signOut } from "firebase/auth";
import { useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { auth } from "../firebase";
import toast from "react-hot-toast";

const LogoutPage = () => {
    const navigate = useNavigate();

    useEffect(()=> {
        signOut(auth)
        .then(() => {
            toast("Logged out!")
            navigate('/Login');
        }).catch((err)=> {
            console.error('error signing out: ', err);
            navigate("/NotFound");
        })
    }, []);
    return <p>Logging out....</p>
}

export default LogoutPage; 