# Dash Delivery

A 2D side-scrolling delivery runner built with **C++17** and **raylib**.

Control a courier, avoid obstacles, collect power-ups, and complete three delivery stages before time or food runs out.

## Features

- Three progressively challenging stages
- Side-scrolling gameplay with parallax backgrounds
- Jumping, gravity, collision detection, and randomized level objects
- Timer, food status, power-ups, stage scores, and final score

## Controls

| Key | Action |
| --- | --- |
| `←` / `→` | Move |
| `Space` | Jump |
| `R` | Restart after failure |
| `Esc` | Exit |

## Obstacles and Power-ups

| Object | Effect |
| --- | --- |
| Roadblock | Lose 10% food, unless protected by a shield |
| Stray Cat | Lose 15% food and move 25% slower for 5 seconds |
| Street Gangster | Food drops to 0%, unless protected by a shield |
| Skates | Move 25% faster for 5 seconds |
| Coupon | Add 10 seconds to the timer |
| Black Tech Box | Block one obstacle effect |
| Delivery Drone | Fly for 5 seconds |

Reach the end of all three stages to win. The mission fails when the timer or food status reaches zero.

## Project Structure

```text
Dash_Delivery/
├── assets/                 # Textures and background music
├── src/
│   ├── main.cpp            # Game loop and input handling
│   ├── levelmanager.cpp    # Stages, rendering, HUD, timer, and scoring
│   └── entities/           # Player, obstacles, and power-ups
└── README.md
```

## Requirements

- macOS
- C++17 or later
- [raylib](https://www.raylib.com/)
- Homebrew

Install raylib:

```bash
brew install raylib
```

## Build and Run

Run these commands from the `src/` directory because the game loads assets using relative paths:

```bash
cd src
clang++ main.cpp levelmanager.cpp entities/Player.cpp \
  -std=c++17 \
  -I"$(brew --prefix raylib)/include" \
  -L"$(brew --prefix raylib)/lib" \
  -lraylib \
  -framework OpenGL -framework Cocoa -framework IOKit \
  -framework CoreVideo -framework AudioToolbox \
  -o dash_delivery
./dash_delivery
```

## Course Project

Created as a group project for **Game Engineering Principles**.
