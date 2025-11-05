const Searchbar = () => {
    return (<div className="w-2/3 h-12 border border-white/20 rounded-2xl antialiased flex items-center px-3 py-2 
        backdrop-blur-md shadow-[0_4px_20px_rgba(0, 0, 0, 0.1)]
        transition-all duration-200 focus-within:shadow-[0_0_0_2px_rgba(59, 130, 246, 0.4)]">
            <input 
                type='text'
                placeholder="search through your projects!"
                className="flex-1 bg-transparent outline-none text-white placeholder-white/60 antialiased selection:bg-blue-500/30"
                 />
        </div>)
}
export default Searchbar;