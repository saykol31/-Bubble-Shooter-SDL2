# 🎮 Bouncing Balls (Bubble Shooter) — C++ / SDL2

[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Library](https://img.shields.io/badge/Library-SDL2-red.svg)](https://libsdl.org/)
[![Course](https://img.shields.io/badge/Sharif%20University-Fundamentals%20of%20Programming%20(25768)-green.svg)](http://sharif.ir/)
[![Total Score](https://img.shields.io/badge/Total%20Score-530%20Pts%20(300%20Base%20%2B%20230%20Bonus)-orange.svg)]()

A feature-rich, high-performance arcade puzzle game inspired by the classic *Bouncing Balls / Bubble Shooter* genre. Built from scratch in **C++** using the **Simple DirectMedia Layer (SDL2)** library as the final project for the **Fundamentals of Programming (Course 25768)** at **Sharif University of Technology** under the supervision of **Dr. Arasteh** and **Dr. Vosooghi Vahdat**.

---

## 📌 Project Documentation References

This project and its feature set are designed and implemented in strict compliance with the official course specifications:
* 📄 **Specification Manual:** [`Basic Programming.pdf`](./Basic%20Programming.pdf) — Complete gameplay mechanics, requirements, and algorithmic constraints.
* 📊 **Evaluation & Rubric Breakdown:** [`BP402_Project_ScoresAllocation.xlsx - Sheet1.csv`](./BP402_Project_ScoresAllocation.xlsx%20-%20Sheet1.csv) — Detailed scoring criteria covering base (300 pts) and bonus (230 pts) milestones.

---

## 🌟 Key Highlights & Gameplay Mechanics

```
  +-------------------------------------------------------------+
  |  [Settings]   [Leaderboard]   [Start Game]   [Pause / Resume]|
  +-------------------------------------------------------------+
  |                                                             |
  |   (●) (●) (●) (●) (●) (●) (●) (●) (●) (●) (●) (●) (●)       | <-- Descending Ball Grid
  |     (●) (●) (●) (★) (●) (●) (●) (🔒) (●) (●) (●)            | <-- Special/Locked Balls
  |       (●) (●) (●) (●) (●) (●) (●) (●) (●)                   |
  |                                                             |
  |                  /                                          |
  |                 /  <-- Aim Trajectory (Wall Reflection)     |
  |                /                                            |
  |             [ 🎯 Cannon ]                                   | <-- Mouse Tracking Cannon
  +-------------------------------------------------------------+
```

### 🎯 Core Mechanics
* **Precision Cannon & Raycasting:**
  * Real-time, zero-latency mouse-tracking cannon positioned at the bottom center of the screen.
  * Dotted aiming trajectory that calculates and visualizes wall bounce reflections.
  * Quick ball swapping using the `SPACE` key.
  * **Adaptive Color Generation Algorithm:** The cannon guarantees only colors currently present on the board are spawned (e.g., if only Red and Green remain, no other base colors are queued).
* **Match & Explosion Physics:**
  * Pop mechanics triggered only when a projectile connects to form a cluster of 3 or more contiguous balls of matching color.
  * Real-time connectivity check: Any unanchored ball or floating island whose path to the top ceiling is severed falls and is eliminated.
  * Smooth wall reflections for projectile trajectories.

---

## 🕹️ Game Modes

The project supports four distinct gameplay modes:

| Mode | Type | Description | Victory / End Condition |
| :--- | :---: | :--- | :--- |
| **Normal / Classic** | Base | Features 5+ hand-crafted layout stages with gradual descending rows. | Clear all balls on the board. |
| **Timed Mode** | Base | Classic mechanics augmented with a countdown clock. | Clear the board before the timer expires. |
| **Random Mode** | Bonus | Procedural grid generator with smart neighbor-affinity algorithms ensuring playable color clusters. | Clear all randomly generated balls. |
| **Endless Mode** | Bonus | Continuous, infinite wave generation. | Ends when balls reach the cannon threshold (Score = total balls popped). |

---

## 🔮 Ball Types, Powers & Special Effects

### 🔴 Base & Obstacle Balls
* **Standard Balls (5+ Colors):** Core puzzle elements requiring 3-match clusters to pop.
* **Black Ball (Indestructible):** Cannot be popped through color matching; destroyed exclusively by severing ceiling anchor connections.
* **Dual-Color Ball (★ Bonus):** Contains two split colors, acting as a wildcard for either color cluster.
* **Locked Ball (★ Bonus):** Encased in an outer shell. A direct hit breaks its lock and recursively unlocks all contiguous locked neighbors.

### ⚡ Cannon Powers & Projectiles
* **Rainbow / Multicolor Ball:** Wildcard projectile that connects and pops any color cluster on contact.
* **Bomb / Explosive Ball (★ Bonus):** High-impact projectile that detonates an area-of-effect (AOE) radius on impact with dedicated explosion animations.
* **Laser Beam (★ Bonus):** Pierces across the entire trajectory line, destroying all balls in its straight-line path with special beam visual effects.
* **Power Bar UI (★ Bonus):** Dedicated on-screen inventory bar to hold, select, and equip special power-up balls on demand.

### ⏱️ Dynamic Time Effects (★ Bonus)
* **Pause Effect:** Freezes the descending grid temporarily; balls struck while paused are immediately destroyed.
* **Slow Motion Effect:** Significantly slows down grid descent speed for easier clears.

---

## 🎨 Menus, Audio & Visual Polish

* **Main & Start Menus:** User profile system, persistent username registration, and game mode selection.
* **Pause Menu (`ESC`):** In-game overlay supporting seamless resume, exit, and real-time audio adjustments.
* **Settings & Customization:**
  * **Audio Control:** Master volume slider, SFX toggles, and background music track switcher.
  * **Theme Switcher:** Customizable background textures and ball skins/color palettes.
* **Persistent Leaderboard:** File I/O system tracking high scores, usernames, and performance stats per game mode.
* **Animation Engine:**
  * Particle and burst animations for popping balls.
  * Gravity-simulated acceleration for disconnected floating balls.
  * Specialized explosion FX and laser beam renders.

---

## 📊 Comprehensive Scoring Matrix (530 Points)

Derived directly from [`BP402_Project_ScoresAllocation.xlsx`](./BP402_Project_ScoresAllocation.xlsx%20-%20Sheet1.csv):

### 1. Mandatory Base Requirements (300 Pts Total)
* **Graphical Initialization & Main Menu:** Window setup (15 pts), Main Menu (15 pts).
* **Settings Menu:** SFX toggles & volume (10 pts), Music controls (10 pts), Config file save/load (5 pts).
* **Leaderboard & Player Profiles:** Leaderboard display (8 pts), Score file persistence (10 pts), Username input (15 pts), Mode selection (10 pts).
* **Base Game Modes:** Normal Mode (20 pts), Timed Mode (15 pts).
* **Core Logic:** Smooth ball movement (10 pts), 3-match cluster popping (10 pts), Disconnected ball elimination (15 pts), Shooting physics (10 pts).
* **Cannon Controls:** Zero-latency mouse tracking (10 pts), Ball swapping (10 pts), Aim line (10 pts).
* **Base Special Elements:** Black balls (5 pts), Rainbow projectile (15 pts).
* **Pause Menu:** Resume & Exit functionality (7 pts).
* **Game Over & Scoring:** Defeat detection & Game Over screen (20 pts), Dynamic score algorithm calculation (20 pts), Win celebration screen (10 pts).

### 2. Bonus & Advanced Implementations (230 Pts Total)
* **Customization:** Theme switcher (Backgrounds & ball skins) (8 pts).
* **Advanced Modes:** Smart Random Grid Mode (20 pts), Endless Survival Mode (35 pts).
* **Physics & VFX Polish:** Wall bounce reflection physics (10 pts), Ball pop particle animations (15 pts), Gravity-based dropping animations (17 pts).
* **Smart Algorithms:** Intelligent cannon color adaptation & queue management (25 pts).
* **Advanced Balls:** Dual-Color Balls (10 pts), Locked Balls with chain-reaction unlocking (15 pts), Balanced special ball distribution (5 pts).
* **Super Weapons & UI:** Bomb Ball with AOE animation (15 pts), Laser Beam with linear destruction FX (20 pts), Power Bar inventory HUD (10 pts).
* **Time Modifiers:** Random Time Effect triggers (10 pts), Pause Grid Effect (5 pts), Slow Motion Effect (5 pts).
* **In-Game Pause Settings:** Audio/Music controls inside the pause overlay (20 pts).

---

## ⌨️ Controls & Keybindings

| Input | Action |
| :--- | :--- |
| **Mouse Cursor** | Aim cannon with dynamic trajectory ray. |
| **Left Mouse Button** | Fire ball / Select menu options. |
| **`SPACE`** | Swap current cannon ball with loaded reserve. |
| **`ESC`** | Toggle In-Game Pause Menu. |

---

## 🛠️ Build & Installation Guide

### Prerequisites
* C++17 or C++20 compatible compiler (`g++`, `clang++`, or MSVC)
* **SDL2 Development Libraries:**
  * `SDL2`
  * `SDL2_image`
  * `SDL2_ttf`
  * `SDL2_mixer`
* `CMake` (v3.16+) or `make`

### Installation Steps (Linux / Ubuntu)
```bash
# 1. Install SDL2 development packages
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

# 2. Clone the repository
git clone [https://github.com/YourUsername/BouncingBalls-SDL2.git](https://github.com/YourUsername/BouncingBalls-SDL2.git)
cd BouncingBalls-SDL2

# 3. Build using CMake
mkdir build && cd build
cmake ..
make

# 4. Run the game
./BouncingBalls
```

### Installation Steps (macOS via Homebrew)
```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer cmake
mkdir build && cd build
cmake ..
make
./BouncingBalls
```

### Installation Steps (Windows - MinGW / MSYS2)
```bash
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer cmake
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
./BouncingBalls.exe
```

---

## 📂 Project Structure

```
├── assets/
│   ├── fonts/           # TrueType font files (.ttf)
│   ├── images/          # Sprites, themes, backgrounds, UI icons
│   └── sounds/          # SFX and background music tracks (.wav, .mp3, .ogg)
├── data/
│   ├── leaderboard.dat  # High scores and user performance records
│   └── settings.cfg     # Saved user preferences and sound levels
├── docs/
│   ├── Basic Programming.pdf                     # Original project description
│   └── BP402_Project_ScoresAllocation.xlsx - Sheet1.csv # Evaluation matrix
├── include/             # Header files (.h / .hpp)
│   ├── Audio.hpp
│   ├── Cannon.hpp
│   ├── Constants.hpp
│   ├── GameEngine.hpp
│   ├── Grid.hpp
│   ├── Particle.hpp
│   ├── PowerUps.hpp
│   └── UI.hpp
├── src/                 # Implementation files (.cpp)
│   ├── Audio.cpp
│   ├── Cannon.cpp
│   ├── GameEngine.cpp
│   ├── Grid.cpp
│   ├── main.cpp
│   ├── Particle.cpp
│   ├── PowerUps.cpp
│   └── UI.cpp
├── CMakeLists.txt
└── README.md
```

---

## 👨‍💻 Academic Credits

* **Institution:** Department of Electrical / Computer Engineering, **Sharif University of Technology**
* **Course:** Fundamentals of Programming (Course ID: 25768)
* **Course Instructors:** Dr. Arasteh & Dr. Vosooghi Vahdat
