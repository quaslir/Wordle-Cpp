import express from "express"
import cors from "cors"
import dotenv from "dotenv"
import mongoose from "mongoose"
import { User } from "../src/models/user.js"
import { WebSocket, WebSocketServer } from "ws"
import { v4 as uuidv4 } from "uuid"
import Groq from "groq-sdk"
import axios from "axios"
import fs from "fs"
import { dirname, join } from "path"
import { fileURLToPath } from "url"
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

interface stopPacket {
    STOP:string
};

const activeRooms = new Map<string, gamePacket>();

const __filename:string = fileURLToPath(import.meta.url);
const __dirname:string = dirname(__filename);
const filePath = join(__dirname, "../assets/sgb-words.txt");

const words:string[] = fs.readFileSync(filePath, 'utf-8').split('\n');
const EnglishDictionary = new Set(words.map(w => w.trim().toLowerCase()));

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


function wordChecker(word: string) : boolean {
return EnglishDictionary.has(word);
}

const EnglishDictionaryToArray:string[] = Array.from(EnglishDictionary);

function getWord():string {
    return EnglishDictionaryToArray[Math.floor(Math.random() * EnglishDictionaryToArray.length)] ?? "";
}




ws.on('connection', async(ws: WebSocket) => {
    queue.push(ws);
    console.log(queue.length);
    if (queue.length >= 2) {
        const p1 = queue.shift();
        const p2 = queue.shift();

        const player1Starts: boolean = Math.round(Math.random()) == 1;
        const roomId: string = uuidv4();
        const word = getWord();
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
            else {
                console.error("PLAYER 1 IS NOT CONNECTED");
                return;
            }

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

        if(msg.toString().includes("STOP")) {
            const targetId:stopPacket = JSON.parse(msg.toString());
            const index: number = queue.indexOf(ws);
            if (index !== -1) {
                queue.splice(index, 1);
            }

            const room = activeRooms.get(targetId.STOP);
            if(room) {
                const opponent = room.player1 === ws ? room.player2 : room.player1;

                if(opponent && opponent.readyState === 1) {
                    
                    opponent.send(JSON.stringify({exit: true}));

                    setTimeout(() => {
                        opponent.close();
                        activeRooms.delete(targetId.STOP);
                    }, 100);
                }
            }
            ws.close();
        } else {

            const packet: receivedPacket = JSON.parse(msg.toString());
            const correctSyntax = wordChecker(packet.word);
                if(!correctSyntax || packet.word.trim().length !== 5) {
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

interface userXp {
    username: string
};

app.post("/getUsersXp", async(req, res) => {
    const {username}:userXp = req.body;
    if(!username) return res.status(404);
    try {
    const target = await User.find({username: username});

    if(!target) return res.status(404);
    return res.json({xp: target[0]?.xp});
    }catch(error) {
        return res.status(500);
    }
});

app.get("/word-gen", (req, res) => {
    return res.json({word: getWord()});
});
interface word_check {
    word:string
};
app.post("/word-check", (req, res) => {
    const {word}:word_check = req.body;
    return res.json({exists: EnglishDictionary.has(word)});
});

app.listen(3000, () => console.log("Server is working on 3000 port"));