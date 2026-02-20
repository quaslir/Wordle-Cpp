import express from "express";
import cors from "cors";
import dotenv from "dotenv";
import mongoose from "mongoose";
import { User } from "../src/models/user.js";
dotenv.config();
const app = express();
app.use(express.json());
app.use(cors());
const url = process.env.MONGO_URL;
const connectDb = async () => {
    try {
        await mongoose.connect(url);
        console.log("MongoDB connected");
    }
    catch (error) {
        console.error("Failed to connect MongoDB");
        process.exit(1);
    }
};
connectDb();
await User.insertOne({ username: "karl", xp: 10 });
app.post("/leaderboard", async (req, res) => {
    console.log(User.find());
});
app.listen(3000, () => console.log("Server is working on 3000 port"));
//# sourceMappingURL=server.js.map