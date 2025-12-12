#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

namespace Constants {

// Window settings
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr const char* WINDOW_TITLE = "SNAKE";

// Grid settings
constexpr int CELL_SIZE = 20;
constexpr int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;   // 40 cells
constexpr int GRID_HEIGHT = (WINDOW_HEIGHT - 60) / CELL_SIZE; // 27 cells (60px for UI)
constexpr int GRID_OFFSET_Y = 60; // Top area for score display

// Game settings
constexpr int TARGET_FPS = 60;
constexpr int FRAME_DELAY = 1000 / TARGET_FPS;
constexpr int INITIAL_SNAKE_LENGTH = 3;
constexpr int INITIAL_GAME_SPEED = 8; // Snake moves every N frames
constexpr int MIN_GAME_SPEED = 3;     // Fastest speed
constexpr int SPEED_INCREASE_INTERVAL = 5; // Speed up every N food eaten

// Controller settings
constexpr int ANALOG_DEAD_ZONE = 8000;

// Colors (RGBA)
namespace Colors {
    // Background
    constexpr uint8_t BG_R = 10, BG_G = 10, BG_B = 26, BG_A = 255;

    // Grid lines
    constexpr uint8_t GRID_R = 26, GRID_G = 26, GRID_B = 42, GRID_A = 255;

    // Snake head (neon green)
    constexpr uint8_t HEAD_R = 57, HEAD_G = 255, HEAD_B = 20, HEAD_A = 255;

    // Snake body (darker green)
    constexpr uint8_t BODY_R = 50, BODY_G = 205, BODY_B = 50, BODY_A = 255;

    // Snake outline
    constexpr uint8_t OUTLINE_R = 0, OUTLINE_G = 100, OUTLINE_B = 0, OUTLINE_A = 255;

    // Food (red)
    constexpr uint8_t FOOD_R = 255, FOOD_G = 0, FOOD_B = 0, FOOD_A = 255;

    // Food glow
    constexpr uint8_t FOOD_GLOW_R = 255, FOOD_GLOW_G = 100, FOOD_GLOW_B = 100, FOOD_GLOW_A = 100;

    // Text (white)
    constexpr uint8_t TEXT_R = 255, TEXT_G = 255, TEXT_B = 255, TEXT_A = 255;

    // Text highlight (neon green)
    constexpr uint8_t HIGHLIGHT_R = 57, HIGHLIGHT_G = 255, HIGHLIGHT_B = 20, HIGHLIGHT_A = 255;

    // Menu selection
    constexpr uint8_t SELECT_R = 100, SELECT_G = 100, SELECT_B = 200, SELECT_A = 255;
}

// File paths
constexpr const char* FONT_PATH = "assets/fonts/pixel.ttf";
constexpr const char* EAT_SOUND_PATH = "assets/sounds/eat.wav";
constexpr const char* GAMEOVER_SOUND_PATH = "assets/sounds/gameover.wav";
constexpr const char* BGM_PATH = "assets/sounds/bgm.ogg";
constexpr const char* HIGHSCORE_PATH = "highscores.json";
constexpr const char* CONTROLLER_DB_PATH = "gamecontrollerdb.txt";

// High score settings
constexpr int MAX_HIGH_SCORES = 10;
constexpr int INITIALS_LENGTH = 3;

// Audio settings
constexpr int AUDIO_FREQUENCY = 44100;
constexpr int AUDIO_CHANNELS = 2;
constexpr int AUDIO_CHUNK_SIZE = 2048;
constexpr int MUSIC_VOLUME = 64;  // 0-128
constexpr int SFX_VOLUME = 100;   // 0-128

} // namespace Constants

// Game states
enum class GameState {
    MENU,
    PLAYER_SELECT,
    ENTER_INITIALS,
    PLAYING,
    PAUSED,
    GAME_OVER,
    HIGH_SCORES,
    PLAYER_SWITCH, // For 2-player mode between turns
    FINAL_RESULTS  // For 2-player mode final comparison
};

// Direction enum
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

// Input action enum
enum class InputAction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SELECT,
    BACK,
    PAUSE,
    NONE
};

// Menu option enum
enum class MenuOption {
    ONE_PLAYER,
    TWO_PLAYERS,
    HIGH_SCORES,
    QUIT
};

#endif // CONSTANTS_H
