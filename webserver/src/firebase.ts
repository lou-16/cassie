import { initializeApp } from "firebase/app";
import { getAuth } from "firebase/auth";

const _apiKey = import.meta.env.VITE_FIREBASE_API_KEY;

const firebaseConfig = {
    apiKey: _apiKey,
    authDomain: "cassie-7b957.firebaseapp.com",
    projectId: "cassie-7b957",
    appId: "cassie-7b957"
};

const App = initializeApp(firebaseConfig);
export const auth = getAuth(App);