import express from "express"
import cors from "cors"
import dotenv from "dotenv"
import mongoose from "mongoose"
import { User } from "../src/models/user.js"
dotenv.config();
const app = express();

app.use(express.json());

app.use(cors());

const url:string = process.env.MONGO_URL as string;
const connectDb = async() => {
    try {
        await mongoose.connect(url);
        console.log("MongoDB connected");
    } catch(error) {
        console.error("Failed to connect MongoDB");
        process.exit(1);
    }
}
connectDb();

app.get("/leaderboard", async(req, res) => {
return res.json({leaderboard: await User.find().select("username xp")});
});

app.listen(3000, ()=> console.log("Server is working on 3000 port"));