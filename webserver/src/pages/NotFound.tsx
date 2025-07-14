export default function NotFound() {
  return (
    <div className="min-h-screen flex flex-col items-center justify-center bg-gray-950 text-white text-center px-4">
      <h1 className="text-6xl font-bold text-purple-500 mb-4">404</h1>
      <p className="text-xl text-gray-300 mb-6">Oops! Page not found.</p>
      <a href="/" className="text-purple-400 hover:underline">
        ← Back to Home
      </a>
    </div>
  );
}
