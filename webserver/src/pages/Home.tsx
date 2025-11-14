
import { Navigate, useNavigate } from "react-router-dom";
import Herotab from "../components/Herotab";
import { useAuth } from "../hooks/useAuth";


function Home() {

  const auth = useAuth()

  return (auth?   
    <div className='bg-transparent min-h-fit flex flex-col overflow-y-hidden w-full'>
      <Herotab />
    </div>
    :
    <Navigate to={"/Dashboard"} />
  )}


export default Home;