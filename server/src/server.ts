import express from "express"
import cors from "cors"
import dotenv from "dotenv"
import mongoose from "mongoose"
import { User } from "../src/models/user.js"
import { WebSocket, WebSocketServer } from "ws"
import { v4 as uuidv4 } from "uuid"
dotenv.config();
const app = express();

app.use(express.json());

app.use(cors());

const ws = new WebSocketServer({port: 8000});

const queue : WebSocket[] = [];

interface startPacket {
turn: boolean,
word: string,
roomId:string
};


interface gamePacket {
    player1:WebSocket,
    player2:WebSocket,
    word:string
};


interface receivedPacket{
    id: string,
    word:string
};

const activeRooms = new Map<string, gamePacket>();


ws.on('connection', (ws: WebSocket) => {
    console.log("New player connected!");
    queue.push(ws);
    if(queue.length >= 2) {
        const p1 = queue.shift();
        const p2 = queue.shift();

        console.log("STARTING GAME...");
        const player1Starts:boolean = Math.round(Math.random()) == 1;
        const roomId:string = uuidv4();
        const packet1:startPacket = {
            turn: player1Starts,
            word: "test",
            roomId: roomId
        };

        const packet2:startPacket = {
            turn: !player1Starts,
            word: "test",
            roomId: roomId
        };
        activeRooms.set(roomId, {
            player1: p1!,
            player2: p2!,
            word: "test"
        });
        setTimeout(() => {
        if(p1?.readyState === WebSocket.OPEN) {
        p1?.send(JSON.stringify({...packet1}), (error) => {
            if(error) {
                console.error(error);
            }
            else console.log("Message was successfully sent!");
        });
        }
        else console.log("PLAYER 1 IS NOT CONNECTED");

       if(p2?.readyState === WebSocket.OPEN) {
        p2?.send(JSON.stringify({...packet2}), (error) => {
            if(error) {
                console.error(error);
            }
            else console.log("Message was successfully sent!");
        });
        }
        else console.log("PLAYER 2 IS NOT CONNECTED");
        }, 200);
    }



    ws.on('message', (msg: string) => {
        console.log(msg.toString());
        if(msg.toString() === "STOP") {
            const index:number = queue.indexOf(ws);
            if(index !== -1) {
                queue.splice(index, 1);
            }
            ws.close();
        }

        else {
            const packet:receivedPacket = JSON.parse(msg.toString());
            const room = activeRooms.get(packet.id);
            if(!room) return;
            const currentUser:WebSocket = ws == room.player1 ? room.player1 : room.player2;
            const nextUser:WebSocket = currentUser == room.player1 ? room.player2 : room.player1;
            nextUser.send(JSON.stringify({word: packet.word, turn: true}));
            currentUser.send(JSON.stringify({turn: false}));
        }

    });

});

ws.on('close', (ws: WebSocket) => {
    console.log("Player disconnected");
    const index:number = queue.indexOf(ws);
            if(index !== -1) {
                queue.splice(index, 1);
            }
});

ws.on('error', console.error);



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
return res.json(await User.find().select("username xp").limit(10));
});

interface updateXp {
    username : string,
    xp: string
};

app.post("/update-leaderboard", async(req, res) => {
    const {username, xp} :updateXp = req.body;
    const toNumberXp:number = Number(xp);
    console.log(username, toNumberXp);
    if(!username || !xp) return res.status(400);

    try {
        const result = await User.updateOne({"username" : username}, {
            $set : {xp: toNumberXp}
        });

        if(result.matchedCount == 0) {
            await User.insertOne({username: username, xp : toNumberXp

            });
        }

        return res.status(200);
    } catch(error) {
        console.error("Failed to update user, error: ", error);
        return res.status(500);
    }
});

app.listen(3000, ()=> console.log("Server is working on 3000 port"));