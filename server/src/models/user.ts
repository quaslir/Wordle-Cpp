import mongo = require("mongoose");
import mongoose = require("mongoose");

interface User {
    username:string,
    xp:number
};

const userSchema = new mongoose.Schema<User>(
    {
        username: {type: String, required: true, unique: true},
        xp: {type: Number, required: true, unique: false, default : 0}
    }
);

export const User = mongoose.model<User>('User', userSchema);