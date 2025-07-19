import React from "react";
import { login } from "../lib/auth";
import toast from "react-hot-toast";
import { useNavigate } from "react-router-dom";

export default function LoginPage() {
  const navigator = useNavigate();
  const [email, setEmail] = React.useState("");
  const [pass, setPass] = React.useState("");

  const handleLogin = async (e: React.FormEvent) => {
    e.preventDefault();
    try {
      await login(email, pass);
      toast("Logged in!");
      navigator("/dashboard")
    } catch (err){
     toast("Login failed");
    }
  }

  return (
    <div className="max-h-screen h-[800px] flex items-center justify-center z-0 ">
      <div className="w-full max-w-md z-10 absolute bg-gray-900 p-8 rounded-xl shadow-lg border border-gray-800">
        <h2 className="text-3xl font-bold text-white text-center mb-6">Login to Cassie</h2>

        <form className="space-y-5" onSubmit={handleLogin}>
          {/* Email */}
          <div>
            <label className="block text-sm font-medium text-gray-300 mb-1" htmlFor="email">
              Email
            </label>
            <input
              type="email"
              id="email"
              placeholder="you@cassie.dev"
              className="w-full px-4 py-2 rounded-md bg-gray-800 border border-gray-700 text-white placeholder-gray-500 focus:outline-none focus:ring-2 focus:ring-purple-500"
              onChange={(e) => setEmail(e.target.value)}
              required
            />
          </div>

          {/* Password */}
          <div>
            <label className="block text-sm font-medium text-gray-300 mb-1" htmlFor="password">
              Password
            </label>
            <input
              type="password"
              id="password"
              placeholder="••••••••"
              className="w-full px-4 py-2 rounded-md bg-gray-800 border border-gray-700 text-white placeholder-gray-500 focus:outline-none focus:ring-2 focus:ring-purple-500"
              onChange={(e) => setPass(e.target.value)}
              required
            />
          </div>

          {/* Submit */}
          <button
            type="submit"
            className="w-full py-2 px-4 rounded-md bg-purple-600 hover:bg-purple-700 transition text-white font-medium"
          >
            Log In
          </button>
        </form>

        {/* Extra */}
        <p className="mt-6 text-sm text-gray-400 text-center">
          Don't have an account?{" "}
          <a href="/signup" className="text-purple-400 hover:underline">
            Sign up
          </a>
        </p>
      </div>
    </div>
  );
}
