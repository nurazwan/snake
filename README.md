# SNAKE GAME

A fun retro-style Snake game you can play alone or with a friend!

## What is this game?

Snake is a classic arcade game where you control a snake that moves around eating food. Every time you eat food, your snake gets longer! The goal is to eat as much food as possible without crashing into the walls or your own tail.

## Features

- **Beautiful retro graphics** - Neon green snake on a dark background, just like old arcade games!
- **1 or 2 players** - Play alone or take turns with a friend to see who gets the highest score
- **High score tracking** - Your best scores are saved so you can try to beat them
- **Works with controllers** - Play with keyboard, PlayStation, Xbox, or any gamepad

## How to Play

### Controls

**Keyboard:**
| Key | Action |
|-----|--------|
| W or Up Arrow | Move Up |
| S or Down Arrow | Move Down |
| A or Left Arrow | Move Left |
| D or Right Arrow | Move Right |
| Enter or Space | Select / Confirm |
| Escape | Back / Quit |
| P | Pause Game |

**PlayStation/Xbox Controller:**
| Button | Action |
|--------|--------|
| D-Pad or Left Stick | Move snake |
| A / X button | Select / Confirm |
| B / Circle button | Back |
| Start | Pause |

### Game Rules

1. Use the arrow keys or WASD to move the snake
2. Eat the red food to grow longer and earn points
3. Don't hit the walls - you'll lose!
4. Don't hit your own tail - you'll lose!
5. The more food you eat, the faster you go
6. Try to get the highest score!

### Scoring

- Each food = 10 points
- Every 5 food eaten, the game speeds up a little
- Your initials and score are saved if you make the top 10!

## Installation

### What you need

- A computer running Windows, macOS, or Linux
- About 10MB of free space
- (Optional) A game controller

### Setting it up

#### macOS

1. Open Terminal (find it in Applications > Utilities)
2. Install Homebrew (a tool that installs other tools):
   ```
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```
3. Install the stuff the game needs:
   ```
   brew install cmake sdl2 sdl2_ttf sdl2_mixer sdl2_image
   ```
4. Go to where you downloaded the game:
   ```
   cd /path/to/snakes
   ```
5. Build the game:
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```
6. Play!
   ```
   ./snake
   ```

#### Windows

1. Download and install Visual Studio from https://visualstudio.microsoft.com/ (get the free Community version)
   - Make sure to check "Desktop development with C++" during install
2. Download and install CMake from https://cmake.org/download/
3. Open PowerShell and install vcpkg:
   ```
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg install sdl2 sdl2-ttf sdl2-mixer sdl2-image
   ```
4. Build the game:
   ```
   cd /path/to/snakes
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg folder]/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```
5. Find snake.exe in the Release folder and run it!

#### Linux (Ubuntu/Debian)

1. Open a terminal
2. Install what you need:
   ```
   sudo apt-get update
   sudo apt-get install build-essential cmake
   sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-image-dev
   ```
3. Build the game:
   ```
   cd /path/to/snakes
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Play!
   ```
   ./snake
   ```

#### Raspberry Pi

Same as Linux! The game is small enough to run great on a Raspberry Pi 2 or newer.

```
sudo apt-get update
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev
mkdir build && cd build
cmake ..
make
./snake
```

## Two Player Mode

In two player mode:
1. Both players enter their 3-letter initials (like arcade games!)
2. Player 1 plays first
3. When Player 1 crashes, they see their score
4. Player 2 plays
5. At the end, both scores are shown and the winner is announced!

## Adding Sound Effects (Optional)

The game can play sounds if you add audio files. Put these in the `assets/sounds/` folder:
- `eat.wav` - plays when eating food
- `gameover.wav` - plays when you crash
- `bgm.ogg` - background music (loops)

You can find free sound effects online at:
- https://freesound.org/
- https://www.zapsplat.com/

## Adding a Cool Font (Optional)

Want the real retro look? Download a pixel font and save it as `assets/fonts/pixel.ttf`

Free pixel fonts: https://www.dafont.com/bitmap.php

## Troubleshooting

**The game won't start:**
- Make sure you installed all the SDL2 libraries
- Try running from the build folder so it can find the assets

**No sound:**
- That's okay! The game works fine without sound
- Check that your audio files are in `assets/sounds/`

**Controller not working:**
- Make sure the controller is plugged in before starting the game
- Try unplugging and plugging it back in
- The game shows "Controller connected" in the terminal when it detects one

**Text looks weird:**
- Download a pixel font and put it in `assets/fonts/pixel.ttf`

## Technical Stuff (for the curious)

- Written in C++17
- Uses SDL2 for graphics, sound, and controller input
- About 2000 lines of code
- Uses less than 10MB of memory
- Runs at 60 frames per second

## Files in this project

```
snakes/
├── src/                    # The game code
│   ├── main.cpp           # Where the program starts
│   ├── Game.cpp/h         # Main game logic
│   ├── Snake.cpp/h        # The snake you control
│   ├── Food.cpp/h         # The food you eat
│   ├── InputManager.cpp/h # Handles keyboard & controllers
│   ├── Renderer.cpp/h     # Draws everything on screen
│   ├── AudioManager.cpp/h # Plays sounds
│   ├── HighScoreManager.cpp/h # Saves your best scores
│   ├── Menu.cpp/h         # Menu navigation
│   └── Constants.h        # Game settings
├── assets/                # Game resources
│   ├── fonts/            # Text fonts
│   ├── sounds/           # Sound effects
│   └── images/           # (not used, for future)
├── CMakeLists.txt        # Build instructions
├── highscores.json       # Your saved high scores
└── README.md             # This file!
```

## Credits

Made with:
- SDL2 - https://www.libsdl.org/
- C++ - a programming language

---

Have fun playing! If you want to learn how the game works, read through the code - it's all commented to help you understand what each part does.
