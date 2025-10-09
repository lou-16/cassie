import { exp } from "three/tsl";
import { db } from "../firebase"
import { auth } from "../firebase"
import {
    doc,
    setDoc,
    getDoc,
    updateDoc,
    arrayRemove,
    arrayUnion
} from "firebase/firestore"

interface UserData {
    name : string;
    email : string;
    createdAt : string;
    containers : [];
    jobs : [];
    projects : string[];
};

export const createUserRecord = async (
    uid : string,
    name : string,
    email : string
) : Promise<void> => {
    const userRef = doc(db, "users", uid);
    await setDoc(userRef, {
        name,
        email,
        createdAt : new Date().toISOString(),
        containers : [],
        jobs : [],
        projects : []
    });
};

export const getUserRecord = async () : Promise<UserData | null> => {
    const user = auth.currentUser;
    if(!user) {
        return null;
    }
    const userRef = doc(db, "users", user.uid);
    const snap = await getDoc(userRef);
    return snap.exists()? snap.data() as UserData : null;
}

export const addUserItem = async (
    type : "containers" | "projects" | "jobs",
    id : string,
) => {
    const user = auth.currentUser;
    if (!user) throw new Error("User not authenticated");

    const userRef = doc(db, "users", user.uid);

    await updateDoc(userRef, {
        [type] : arrayUnion(id),
    });
};

export const removeUserItem = async (
    type : "containers" | "projects" | "jobs",
    id : string 
) : Promise<void> => {
    const user = auth.currentUser
    if(!user) throw new Error("User not authenticated");

    const userRef = doc(db, "users", user.uid);

    await updateDoc(userRef, {
        [type] : arrayRemove(id),
    });
};

export default db;