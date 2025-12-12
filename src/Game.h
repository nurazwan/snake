#ifndef GAME_H
#define GAME_H

#include <string>
#include <memory>
#include "Constants.h"
#include "Snake.h"
#include "Food.h"
#include "InputManager.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "HighScoreManager.h"
#include "Menu.h"

struct PlayerData {
    std::string initials;
    int score;

    PlayerData() : score(0) {}

    void reset() {
        initials.clear();
        score = 0;
    }
};

class Game {
public:
    Game();
    ~Game();

    // Initialize all game systems
    bool init();

    // Main game loop
    void run();

    // Cleanup
    void shutdown();

private:
    // State update methods
    void update();
    void render();

    // State-specific update methods
    void updateMenu();
    void updatePlayerSelect();
    void updateEnterInitials();
    void updatePlaying();
    void updatePaused();
    void updateGameOver();
    void updateHighScores();
    void updatePlayerSwitch();
    void updateFinalResults();

    // State-specific render methods
    void renderMenu();
    void renderPlayerSelect();
    void renderEnterInitials();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();
    void renderHighScores();
    void renderPlayerSwitch();
    void renderFinalResults();

    // Game logic helpers
    void startNewGame();
    void resetCurrentPlayer();
    void handleGameOver();
    void switchToNextPlayer();

    // Change game state
    void setState(GameState newState);

    // Components
    std::unique_ptr<InputManager> input;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<AudioManager> audio;
    std::unique_ptr<HighScoreManager> highScores;
    std::unique_ptr<Menu> menu;

    // Game objects
    Snake snake;
    Food food;

    // Game state
    GameState currentState;
    bool running;

    // Player data
    int numPlayers;
    int currentPlayer; // 1 or 2
    PlayerData players[2];

    // Current game session
    int score;
    int gameSpeed;
    int moveTimer;
    int foodEaten;
    bool newHighScore;

    // Frame timing
    Uint32 frameStart;
    int frameTime;
};

#endif // GAME_H
