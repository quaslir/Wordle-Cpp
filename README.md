# Wordle-Cpp

![Screenshot](/Screens/screen1.png) ![Screenshot](/Screens/screen2.png) ![Screenshot](/Screens/screen3.png) ![Screenshot](/Screens/screen4.png) ![Screenshot](/Screens/screen5.png)

## Overview

**Wordly-C++** is a desktop clone of the popular `Wordle` game, built using **C++20** and the **raylib** library. The game challenges you to guess a hidden 5-letter word in 6 attempts with real-time color feedback.

## Features

* **Dynamic Dictionary:** Loads almost 6000 popular 5-letter English words.
* **Anti-Cheat System:** Only valid dictionary words are accepted as guesses.
* **Cross-platform:** Easy to build using CMake.
* **Autoplay Mode:** Intelligent AI solver to assist or play for you.
* **User Statistics:** Tracks `current streak`, `total wins`, `win distribution`, `total lost games`, and `best streak`.
* **Dual Play Modes:**
    * **Single Player:** Relaxed offline experience with persistent statistics and AI assistance.
    * **Real-time PvP:** Competitive online matches using a custom socket-based protocol for low-latency synchronization.
* **Smart Audio Manager:** Fully integrated background music system and event-driven sound effects (Victory/Battle) with automated playlist rotation.
* **Global Leaderboards:** Competitive ranking system with asynchronous data fetching to track top players' XP worldwide.

---

## 🤖 Autoplay Algorithm

The game features an intelligent solver that attempts to crack the code using a **Heuristic Filtering Strategy**:

1.  **Initial Seed:** The bot starts with a randomly selected valid 5-letter word to ensure variety in every session.
2.  **Constraint Analysis:** After each guess, the bot analyzes the feedback:
    * 🟩 **Fixed Constraints:** Locks in letters identified in the correct positions.
    * 🟨 **Existential Constraints:** Retains letters that must be present but at different indices.
    * ⬜ **Exclusionary Constraints:** Filters out any words containing "gray" letters.
3.  **Dictionary Pruning:** The bot performs a real-time sweep of the dictionary, removing all words that violate the current constraints and picking the next candidate from the remaining pool.



---

## Getting Started

### Prerequisites
* A C++20 compatible compiler (GCC, Clang, MSVC).
* CMake 3.10 or higher.
* Git.

### Installation & Build
```bash
# Clone the repository
git clone https://github.com/quaslir/Wordle-Cpp
cd Wordly-C-

# Initialize raylib submodule
git submodule update --init --recursive

# Build the project
mkdir build && cd build
cmake ..
make

# Run the game
./Wordle-Cpp

```

**⚠️ Important** 
#### In the root directory, there are two important files: `history.json` and `sgb-words.txt`. 

* The first will be created automatically and stores history data; deleting this file will cause all your history to be reset to zero. 
* The second file is the main dictionary; if you delete this file or move it to another directory, game will use online dictionary, and you won't be able to play offline mode.

```bash
./Wordle-Cpp
```

---

## Controls

| Input | Action |
| :--- | :--- |
| **[A-Z]** | Type letters |
| **[Backspace]** | Remove last letter |
| **[Enter]** | Submit word for validation |
| **[Mouse]** | Navigate "Play Again" or "Exit" buttons |

## How It Works

Wordly-C++ uses a strict validation logic. To prevent **"vowel-spamming"** (e.g., typing `AEIOU`), every entry is checked against the internal dictionary. If a word doesn't exist in the database, it won't be accepted.

### Feedback System


* 🟩 **Green**: Letter is in the word and in the correct spot.
* 🟨 **Yellow**: Letter is in the word but in the wrong spot.
* ⬜ **Gray**: Letter is not in the word at all.

---

### ⚙️ Settings & Customization

The settings menu allows you to tailor the game experience to your preference. Fine-tune your gameplay with the following options:

* **Hard Mode:** For those seeking a greater challenge. Enabling this mode obliges the user to use all revealed hints (green and yellow letters) in all following guess attempts.
* **Offline Mode:** Enable this to use a local dictionary instead of the remote word provider. 
    * **Note:** In Offline Mode, the **Global Leaderboard** and **PvP Mode** are disabled to ensure data integrity and synchronization.
* **Music Volume:** Adjust the game's atmosphere with a dedicated volume slider, controlling the playback levels of the smart audio manager.

---

### 🔒 Security & Data Integrity

To ensure fair play and protect the integrity of player progress, the game implements a multi-layered security system for local data:

* **XOR Encryption:** User statistics and sensitive local data are stored in a `.json` format, but are fully encrypted using an **XOR cipher**. This prevents casual snooping or direct editing of the file.
* **Integrity Checksum:** The system generates a unique **checksum** for the data file. Every time the game loads, it validates the current file against this signature to identify any unauthorized external modifications.
* **Anti-Tamper Protocol:** If the game detects that the stats file has been tampered with or the checksum fails to match, a "Reset on Detection" policy is triggered. **All user statistics will be immediately reset to zero** to prevent cheating.

---

### ⚔️ PvP Mode: Real-Time Duel

Experience a high-stakes competitive mode where speed and logic are your only weapons. In **PvP Mode**, you go head-to-head with another player in real-time.

* **Shared Word Space:** Both players guess the exact same hidden word simultaneously.
* **Winner Takes All:** The first player to successfully guess the word claims the victory and receives **XP**. The opponent fails the challenge.
* **Networking Stack:** * **Client:** Built from the ground up using **C++ Web-Sockets**.
    * **Central Server:** A high-performance backend built with **TypeScript**, handling match-making and packet synchronization.

#### 🛠️ Running a Local Server
If you want to host your own match-making instance:
1. Navigate to the `server/` directory.
2. Install dependencies: `npm install`.
3. Start the development server:
   ```bash
   
   npm run dev

*Created by **Karl** as a Computer Science practice project.*
