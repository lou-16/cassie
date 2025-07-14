import CopyBlock from "../CopyBlock";
import ThreeCanvas from "./ThreeCanvas";

const Herotab = () => {
  return (
    <div className="relative w-full h-[800px] overflow-hidden font-sans text-white">


      <div className="z-10 absolute inset-0 py-10 items-center justify-center h-full text-center flex flex-row">
        <div>
            <h1 className="text-center text-5xl md:text-6xl font-extrabold mb-4 drop-shadow-[0_0_10px_rgba(255,255,255,0.2)]">
            Deploy Smarter with <span className="text-glow">Cassie</span>
            </h1>
            <p className="text-lg p-12 text-gray-300 max-w-xl mb-8 px-4">
            A blazing-fast PaaS solution for containerized, CLI-powered cloud
            apps.
            </p>
        </div>
        <div className="w-1 rounded-sm mx-24 bg-gray-500 h-full "></div>
        <div className="flex flex-col gap-4">
          <CopyBlock text={"cassie --deploy myapp --port 3000"}/>
          <button className="bg-accent px-6 py-3 rounded-xl font-medium hover:shadow-glow transition">
            Get Started
          </button>
          <button className="bg-transparent border border-white px-6 py-3 rounded-xl font-medium hover:bg-white hover:text-black transition">
            GitHub
          </button>
        </div>
      </div>
    </div>
  );
};

export default Herotab;
