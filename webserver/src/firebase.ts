import { initializeApp } from "firebase/app";
import { getAuth } from "firebase/auth";
import { getFirestore } from "firebase/firestore";
import { append } from "three/tsl";
const _apiKey = import.meta.env.VITE_FIREBASE_API_KEY;
const firebaseConfig = {
  apiKey: _apiKey,
  authDomain: "cassie-7b957.firebaseapp.com",
  projectId: "cassie-7b957",
  storageBucket: "cassie-7b957.firebasestorage.app",
  messagingSenderId: "48781077332",
  appId: "1:48781077332:web:49701ecc18acf06337b6c1"
};

const App = initializeApp(firebaseConfig);
export const auth = getAuth(App);
export const db = getFirestore(App);