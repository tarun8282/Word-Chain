# 🔗 Word-Chain Game (C)

A **console-based Word Chain game** written in C.  
Test your vocabulary by linking words where each word starts with the **last letter** of the previous one.  
Features a timer, scoring system, and dictionary validation for a challenging gameplay experience.

---

## ✨ Features

✅ **Random Start Word** – game begins with a random word from the dictionary  
✅ **Dictionary Validation** – rejects words not in `dictionary.txt`  
✅ **No Repeats** – uses a hash table to track used words  
✅ **Timer** – 10-second limit per turn, displayed live  
✅ **Formatted Console UI** – clean box-style display  
✅ **Score Tracking** – earn points based on word length  
✅ **Pause / Resume Support** (Optional if you’ve added it)

---

## 📂 Project Structure

Word-Chain/
├── chain.c # Main game logic
├── dictionary.txt # Word list (one per line)
├── README.md # This file

yaml
Copy code

---

## ⚡ Getting Started

### 1️⃣ Requirements

- C compiler (`gcc`, `clang`, or MSVC)
- `dictionary.txt` in the same directory
- On Windows: `conio.h` and `windows.h` are used

### 2️⃣ Build & Run

```bash
gcc chain.c -o wordchain
./wordchain
(Use wordchain.exe on Windows)

🎮 How to Play
The game starts with a random word from the dictionary.

Enter a word that starts with the last letter of the current word.

You have 10 seconds to type a valid word.

Words cannot be repeated.

The game ends if:

You run out of time ⏳

Your word is not in the dictionary ❌

Your word does not start with the correct letter 🅰️

You repeat a previously used word 🔁

Your final score and reason for game over are displayed.

🖼️ Example Gameplay
mathematica
Copy code
    ╔═════════════════════════════════════════╗
    ║                 G A M E   O N           ║
    ╠═════════════════════════════════════════╣
    ║  Turn: 1           Current Score: 0     ║
    ╠═════════════════════════════════════════╣
    ║                                         ║
    ║   Current Word:   apple                 ║
    ║   Next word must start with: 'E'        ║
    ║                                         ║
    ╚═════════════════════════════════════════╝

    Time: 09 sec | Your Word > elephant
🚀 Future Enhancements
Multiplayer mode (Player vs Player or Player vs Computer)

Persistent high-score leaderboard

Colored console output for better UX

Option to load dictionary from an online source

Save/Load feature for resuming sessions

📜 License
This project is open-source. You can add your preferred license (e.g., MIT, GPL).

💡 Author
👤 Tarun Chaudhary
📌 GitHub Profile
