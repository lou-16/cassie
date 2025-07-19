/** @type {import('tailwindcss').Config} */

export default {
    content : [
        "./src/**/*.{js,ts,jsx,tsx}",
    ],
    theme : {
        extend : {
            colors : {
                glow: "#a855f7", 
                background: "#0f0f1a",
                accent: '#22d3ee', 
            },
            fontFamily: {
                sans: ['Inter', 'sans-serif'],
            },
            boxShadow: {
                glow: '0 0 10px #a855f7',
            },
        }
    },
    plugins : [],
}