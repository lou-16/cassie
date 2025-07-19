export default function Footer() {
  return (
    <footer className="w-full bg-gray-900 text-gray-300 px-6 py-10 bottom-0">
      <div className="max-w-screen-xl mx-auto grid grid-cols-1 md:grid-cols-3 gap-8 text-sm">
        {/* Column 1: About */}
        <div>
          <h3 className="text-white text-lg font-semibold mb-4">About Cassie</h3>
          <p className="text-gray-400">
            Cassie is a developer-first PaaS that simplifies container deployments with a single command.
          </p>
        </div>

        {/* Column 2: Links */}
        <div>
          <h3 className="text-white text-lg font-semibold mb-4">Quick Links</h3>
          <ul className="space-y-2">
            <li><a href="#docs" className="hover:text-white">Documentation</a></li>
            <li><a href="#cli" className="hover:text-white">CLI Reference</a></li>
            <li><a href="#pricing" className="hover:text-white">Pricing</a></li>
            <li><a href="#blog" className="hover:text-white">Blog</a></li>
          </ul>
        </div>

        {/* Column 3: Contact */}
        <div>
          <h3 className="text-white text-lg font-semibold mb-4">Contact</h3>
          <ul className="space-y-2">
            <li>Email: <span className="text-white">gurnoor16singh@gmail.com</span></li>
            <li>GitHub: <a href="https://github.com/lou-16" className="hover:text-white">github.com/lou-16</a></li>
            <li>Address: <span className="text-gray-400">yeah bro i aint putting that up here </span></li>
          </ul>
        </div>
      </div>

      {/* Bottom copyright */}
      <div className="mt-10 border-t border-gray-700 pt-6 text-center text-xs text-gray-500">
        © {new Date().getFullYear()} Cassie — All rights reserved.
      </div>
    </footer>
  );
}
