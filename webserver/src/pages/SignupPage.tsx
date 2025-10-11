import { Link, useNavigate } from "react-router-dom";
import { useState } from "react";
import { signup } from "../lib/auth";
import toast from "react-hot-toast";

function SignupPage() {
  const navigator = useNavigate()
  const [formData, setFormData] = useState({
    name: "",
    email: "",
    password: "",
    confirmPassword: "",
  });

  const [error, setError] = useState({
    isError : false,
    ErrorText : "",
  })

  const handleChange = (e : any) => {
    setFormData({ ...formData, [e.target.name]: e.target.value });
  };

const handleSubmit = async (e : any) => {
  e.preventDefault();
  setError({ isError: false, ErrorText: "" }); // reset before validation

  if (formData.password !== formData.confirmPassword) {
    toast("Password and Confirm Password must match.");
    setError({ isError: true, ErrorText: "auth/password-and-confirm-no-match" });
    return; // stop here if mismatch
  }

  try {
    const userCredentials = await signup(formData.email, formData.password);
    localStorage.setItem("userCreds" ,userCredentials.user.toJSON.toString()) // ✅ wait for Firebase
    toast("SignUp successful!");
    navigator("/Dashboard"); // ✅ only navigate if success
  } catch (err : any) {
    // handle firebase errors
    if (err.code === "auth/email-already-in-use") {
      toast("An account with this email already exists!");
      setError({ isError: true, ErrorText: err.code });
    } else if (err.code === "auth/invalid-email") {
      toast("Please enter a valid email address!");
      setError({ isError: true, ErrorText: err.code });
    } else if (err.code === "auth/weak-password") {
      toast("Password should be at least 6 letters");
      setError({ isError: true, ErrorText: err.code });
    } else {
      toast("Something went wrong, please try again.");
      setError({ isError: true, ErrorText: "auth/unhandled-error" });
    }
  }
};


  return (
    <div className="flex items-center justify-center min-h-screen bg-[#0a0a0a]">
      <div className="bg-[#0f172a] text-white rounded-2xl shadow-xl p-10 w-[400px]">
        <h2 className="text-2xl font-bold text-center mb-6">Sign up to Cassie</h2>

        <form onSubmit={handleSubmit} className="flex flex-col space-y-4">
          <div>
            <label className="text-sm mb-1 block">Full Name</label>
            <input
              type="text"
              name="name"
              value={formData.name}
              onChange={handleChange}
              className="w-full p-3 rounded-md bg-[#1e293b] text-white focus:outline-none focus:ring-2 focus:ring-purple-600"
              required
            />
          </div>

          <div>
            <label className="text-sm mb-1 block">Email</label>
            <input
              type="email"
              name="email"
              value={formData.email}
              onChange={handleChange}
              className="w-full p-3 rounded-md bg-[#1e293b] text-white focus:outline-none focus:ring-2 focus:ring-purple-600"
              required
            />
          </div>

          <div>
            <label className="text-sm mb-1 block">Password</label>
            <input
              type="password"
              name="password"
              value={formData.password}
              onChange={handleChange}
              className="w-full p-3 rounded-md bg-[#e2e8f0] text-black focus:outline-none focus:ring-2 focus:ring-purple-600"
              required
            />
          </div>

          <div>
            <label className="text-sm mb-1 block">Confirm Password</label>
            <input
              type="password"
              name="confirmPassword"
              value={formData.confirmPassword}
              onChange={handleChange}
              className="w-full p-3 rounded-md bg-[#e2e8f0] text-black focus:outline-none focus:ring-2 focus:ring-purple-600"
              required
            />
          </div>

          <button
            type="submit"
            className="mt-4 w-full bg-purple-600 hover:bg-purple-700 text-white font-semibold py-3 rounded-md transition-colors"
          >
            Sign Up
          </button>
        </form>

        <p className="text-sm text-center mt-4 text-gray-400">
          Already have an account?{" "}
          <Link to="/login" className="text-purple-400 hover:underline">
            Log in
          </Link>
        </p>
      </div>
    </div>
  );
}

export default SignupPage;
