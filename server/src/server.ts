import express from "express"
import cors from "cors"
import dotenv from "dotenv"
import mongoose from "mongoose"
import { User } from "../src/models/user.js"
import { WebSocket, WebSocketServer } from "ws"
import { v4 as uuidv4 } from "uuid"
import Groq from "groq-sdk"
import axios from "axios"
dotenv.config();
const app = express();
const groq = new Groq({ apiKey: process.env.GROK_KEY });
app.use(express.json());

app.use(cors());

const ws = new WebSocketServer({ port: 8000 });

const queue: WebSocket[] = [];

interface startPacket {
    turn: boolean,
    word: string,
    roomId: string
};


interface gamePacket {
    player1: WebSocket,
    player2: WebSocket,
    word: string,
    totalAttempts: number
};


interface receivedPacket {
    id: string,
    word: string
};

const activeRooms = new Map<string, gamePacket>();


async function getLLMData(prompt: string): Promise<string> {
try {
        const chatCompletion = await groq.chat.completions.create({
            "messages": [
                {
                    "role": "system",
                    "content": prompt
                }

            ],
            "model": "llama-3.1-8b-instant",
            "temperature": 0,
            "max_completion_tokens": 200,
            "top_p": 1,
            "stream": false,
            "stop": null
        });
        const reply = chatCompletion.choices[0]?.message?.content || "";
        return reply;
    } catch(error) {
        console.error(error);
        return "";
    }
}


async function wordChecker(word: string) : Promise<boolean> {
try {
    const res = await axios.get(`https://api.dictionaryapi.dev/api/v2/entries/en/${word}`);
    return true;
} catch(error) {
    return false;
}
}


async function getWord():Promise<string> {
    const alphabet:string = "abcdefghijklmnopqrstuvwxyz";
    const randomChar:string = alphabet[Math.floor(Math.random() * alphabet.length)]!;
    try {
        const reply = await getLLMData(`Generate a common 5-letter English word that starts with the letter '${randomChar}'. 
    Output ONLY the word in lowercase.`);
        if(reply.length !== 5 || (!wordChecker(reply))) {
            return getWord();
        }
        return reply;
    } catch(error) {
        console.error(error);
        return "";
    }
}




ws.on('connection', async(ws: WebSocket) => {
    queue.push(ws);
    if (queue.length >= 2) {
        const p1 = queue.shift();
        const p2 = queue.shift();

        const player1Starts: boolean = Math.round(Math.random()) == 1;
        const roomId: string = uuidv4();
        const word = await getWord();
        console.log(word);
        const packet1: startPacket = {
            turn: player1Starts,
            word: word,
            roomId: roomId
        };

        const packet2: startPacket = {
            turn: !player1Starts,
            word: word,
            roomId: roomId
        };
        activeRooms.set(roomId, {
            player1: p1!,
            player2: p2!,
            word: word,
            totalAttempts: 1
        });
        setTimeout(() => {
            if (p1?.readyState === WebSocket.OPEN) {
                p1?.send(JSON.stringify({ ...packet1 }), (error) => {
                    if (error) {
                        console.error(error);
                    }
                    else console.log("Message was successfully sent!");
                });
            }
            else console.log("PLAYER 1 IS NOT CONNECTED");

            if (p2?.readyState === WebSocket.OPEN) {
                p2?.send(JSON.stringify({ ...packet2 }), (error) => {
                    if (error) {
                        console.error(error);
                    }
                    else console.log("Message was successfully sent!");
                });
            }
            else console.log("PLAYER 2 IS NOT CONNECTED");
        }, 200);
    }



    ws.on('message', async(msg: string) => {
        console.log(msg.toString());
        if (msg.toString() === "STOP") {
            const index: number = queue.indexOf(ws);
            if (index !== -1) {
                queue.splice(index, 1);
            }
            ws.close();
        }

        else {
            const packet: receivedPacket = JSON.parse(msg.toString());
            const correctSyntax = await wordChecker(packet.word);
                if(!correctSyntax|| packet.word.trim().length !== 5) {
                ws.send(JSON.stringify({incorrect: true}));
                return;
            }
            const room = activeRooms.get(packet.id);
            if (!room) return;

            const currentUser: WebSocket = ws == room.player1 ? room.player1 : room.player2;
            const nextUser: WebSocket = currentUser == room.player1 ? room.player2 : room.player1;

            if (packet.word === room.word) {
                currentUser.send(JSON.stringify({ turn: false, win: true, gameOver: true }));
                nextUser.send(JSON.stringify({ turn: false, win: false, gameOver: true, draw: false }));
            }
            else if (room.totalAttempts === 6) {
                currentUser.send(JSON.stringify({ turn: false, win: false, gameOver: true, draw: true }));
                nextUser.send(JSON.stringify({ turn: false, win: false, gameOver: true, draw: true }));
            }
            else {
                room.totalAttempts++;

                nextUser.send(JSON.stringify({ word: packet.word, turn: true }));
                currentUser.send(JSON.stringify({ turn: false }));
            }
        }

    });

});

ws.on('close', (ws: WebSocket) => {
    console.log("Player disconnected");
    const index: number = queue.indexOf(ws);
    if (index !== -1) {
        queue.splice(index, 1);
    }
});

ws.on('error', console.error);



const url: string = process.env.MONGO_URL as string;
const connectDb = async () => {
    try {
        await mongoose.connect(url);
        console.log("MongoDB connected");
    } catch (error) {
        console.error("Failed to connect MongoDB");
        process.exit(1);
    }
}
connectDb();
app.get("/leaderboard", async (req, res) => {
    return res.json(await User.find().select("username xp").limit(10));
});

interface updateXp {
    username: string,
    xp: string
};

app.post("/update-leaderboard", async (req, res) => {
    const { username, xp }: updateXp = req.body;
    const toNumberXp: number = Number(xp);
    console.log(username, toNumberXp);
    if (!username || !xp) return res.status(400);

    try {
        const result = await User.updateOne({ "username": username }, {
            $set: { xp: toNumberXp }
        });

        if (result.matchedCount == 0) {
            await User.insertOne({
                username: username, xp: toNumberXp

            });
        }

        return res.status(200);
    } catch (error) {
        console.error("Failed to update user, error: ", error);
        return res.status(500);
    }
});

app.listen(3000, () => console.log("Server is working on 3000 port"));