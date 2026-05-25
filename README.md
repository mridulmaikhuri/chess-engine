# ♟️ Chess Engine

A modern chess engine and graphical user interface built in C++ using SDL3.  
This project focuses on building a fully interactive chess application from scratch, including board rendering, move validation and develop a fully autonomous chess engine which can play the game with a human.


## ✨ Features

- ♜ Chess board rendering
- ♞ Piece movement

---

## 🛠️ Tech Stack

- C++
- SDL3
- SDL3_image
- CMake

---

## 📁 Project Structure

```text
project/
│
├── assets/               # Chess piece images and other assets
│
├── src/                  # Source files
│   ├── main.cpp
│   ├── board.cpp         # Stores the state of the board in a class
│   ├── board.h
│
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

## 🚀 How to Run

### 1️⃣ Clone the Repository

```bash
git clone <your-repo-url>
cd project
```

### 2️⃣ Create Build Directory

```bash
mkdir build
cd build
```

### 3️⃣ Generate Build Files

```bash
cmake ..
```

### 4️⃣ Compile the Project

```bash
make
```

### 5️⃣ Run the Executable

```bash
./app
```