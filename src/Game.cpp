#include "Game.h"
#include <cstdio>

Game::Game()
    : currentState(GameState::MENU)
    , running(false)
    , numPlayers(1)
    , currentPlayer(1)
    , score(0)
    , gameSpeed(Constants::INITIAL_GAME_SPEED)
    , moveTimer(0)
    , foodEaten(0)
    , newHighScore(false)
    , frameStart(0)
    , frameTime(0) {
}

Game::~Game() {
    shutdown();
}

bool Game::init() {
    // Create components
    input = std::make_unique<InputManager>();
    renderer = std::make_unique<Renderer>();
    audio = std::make_unique<AudioManager>();
    highScores = std::make_unique<HighScoreManager>();
    menu = std::make_unique<Menu>();

    // Initialize renderer first (creates window)
    if (!renderer->init()) {
        printf("Error: Failed to initialize renderer\n");
        return false;
    }

    // Initialize input
    if (!input->init()) {
        printf("Warning: Input initialization had issues\n");
        // Continue anyway - keyboard should still work
    }

    // Initialize audio (optional - game works without sound)
    if (!audio->init()) {
        printf("Warning: Audio initialization failed - continuing without sound\n");
    }

    // Load high scores
    if (!highScores->load()) {
        printf("Warning: Could not load high scores\n");
    }

    // Start background music
    audio->playBackgroundMusic();

    running = true;
    currentState = GameState::MENU;
    menu->setNumOptions(4);

    printf("Game initialized successfully!\n");
    return true;
}

void Game::shutdown() {
    if (audio) audio->shutdown();
    if (input) input->shutdown();
    if (renderer) renderer->shutdown();

    audio.reset();
    input.reset();
    renderer.reset();
    highScores.reset();
    menu.reset();

    running = false;
}

void Game::run() {
    while (running) {
        frameStart = SDL_GetTicks();

        // Process input
        if (!input->processEvents()) {
            running = false;
            break;
        }

        // Update game state
        update();

        // Render
        render();

        // Clear single-frame input flags
        input->clearFrameFlags();

        // Frame rate limiting
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < Constants::FRAME_DELAY) {
            SDL_Delay(Constants::FRAME_DELAY - frameTime);
        }
    }
}

void Game::setState(GameState newState) {
    currentState = newState;

    // State entry actions
    switch (newState) {
        case GameState::MENU:
            menu->reset();
            menu->setNumOptions(4);
            break;

        case GameState::PLAYER_SELECT:
            menu->reset();
            menu->setNumOptions(2);
            break;

        case GameState::ENTER_INITIALS:
            menu->resetInitials();
            SDL_StartTextInput();
            break;

        case GameState::PLAYING:
            SDL_StopTextInput();
            break;

        case GameState::GAME_OVER:
            audio->playGameOverSound();
            break;

        default:
            break;
    }
}

void Game::update() {
    switch (currentState) {
        case GameState::MENU:
            updateMenu();
            break;
        case GameState::PLAYER_SELECT:
            updatePlayerSelect();
            break;
        case GameState::ENTER_INITIALS:
            updateEnterInitials();
            break;
        case GameState::PLAYING:
            updatePlaying();
            break;
        case GameState::PAUSED:
            updatePaused();
            break;
        case GameState::GAME_OVER:
            updateGameOver();
            break;
        case GameState::HIGH_SCORES:
            updateHighScores();
            break;
        case GameState::PLAYER_SWITCH:
            updatePlayerSwitch();
            break;
        case GameState::FINAL_RESULTS:
            updateFinalResults();
            break;
    }
}

void Game::render() {
    renderer->clear();

    switch (currentState) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::PLAYER_SELECT:
            renderPlayerSelect();
            break;
        case GameState::ENTER_INITIALS:
            renderEnterInitials();
            break;
        case GameState::PLAYING:
            renderPlaying();
            break;
        case GameState::PAUSED:
            renderPlaying(); // Draw game state
            renderPaused();  // Draw overlay
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::HIGH_SCORES:
            renderHighScores();
            break;
        case GameState::PLAYER_SWITCH:
            renderPlayerSwitch();
            break;
        case GameState::FINAL_RESULTS:
            renderFinalResults();
            break;
    }

    renderer->present();
}

// === State Update Methods ===

void Game::updateMenu() {
    if (menu->handleInput(*input)) {
        int option = menu->getSelectedOption();

        switch (option) {
            case 0: // 1 Player
                numPlayers = 1;
                currentPlayer = 1;
                players[0].reset();
                setState(GameState::ENTER_INITIALS);
                break;

            case 1: // 2 Players
                numPlayers = 2;
                currentPlayer = 1;
                players[0].reset();
                players[1].reset();
                setState(GameState::ENTER_INITIALS);
                break;

            case 2: // High Scores
                setState(GameState::HIGH_SCORES);
                break;

            case 3: // Quit
                running = false;
                break;
        }
    }

    if (menu->isBackPressed()) {
        running = false;
    }
}

void Game::updatePlayerSelect() {
    if (menu->handleInput(*input)) {
        int option = menu->getSelectedOption();
        numPlayers = option + 1;
        currentPlayer = 1;
        players[0].reset();
        if (numPlayers == 2) players[1].reset();
        setState(GameState::ENTER_INITIALS);
    }

    if (menu->isBackPressed()) {
        menu->clearBackPressed();
        setState(GameState::MENU);
    }
}

void Game::updateEnterInitials() {
    if (menu->handleInitialsInput(*input)) {
        // Store initials for current player
        players[currentPlayer - 1].initials = menu->getInitials();

        // Check if we need more players
        if (numPlayers == 2 && currentPlayer == 1) {
            currentPlayer = 2;
            menu->resetInitials();
        } else {
            // All initials entered, start game
            currentPlayer = 1;
            startNewGame();
        }
    }

    if (menu->isBackPressed()) {
        menu->clearBackPressed();
        SDL_StopTextInput();

        if (currentPlayer == 2) {
            // Go back to player 1 entry
            currentPlayer = 1;
            menu->resetInitials();
            SDL_StartTextInput();
        } else {
            setState(GameState::MENU);
        }
    }
}

void Game::updatePlaying() {
    // Handle pause
    if (input->isPausePressed()) {
        setState(GameState::PAUSED);
        audio->pauseBackgroundMusic();
        return;
    }

    // Handle direction input
    Direction dir = input->getDirection();
    if (dir != Direction::NONE) {
        snake.setDirection(dir);
    }

    // Update food animation
    food.update();

    // Move snake at game speed
    moveTimer++;
    if (moveTimer >= gameSpeed) {
        moveTimer = 0;

        snake.move();

        // Check collisions
        if (snake.checkWallCollision() || snake.checkSelfCollision()) {
            handleGameOver();
            return;
        }

        // Check food collision
        if (snake.getHead() == food.getPosition()) {
            snake.grow();
            score += 10;
            foodEaten++;
            audio->playEatSound();

            // Speed up game
            if (foodEaten % Constants::SPEED_INCREASE_INTERVAL == 0) {
                if (gameSpeed > Constants::MIN_GAME_SPEED) {
                    gameSpeed--;
                }
            }

            // Spawn new food
            food.spawn(snake);
        }
    }
}

void Game::updatePaused() {
    if (input->isPausePressed() || input->isSelectPressed()) {
        setState(GameState::PLAYING);
        audio->resumeBackgroundMusic();
    }

    if (input->isBackPressed()) {
        // Quit current game
        setState(GameState::MENU);
        audio->resumeBackgroundMusic();
    }
}

void Game::updateGameOver() {
    if (input->isSelectPressed()) {
        if (numPlayers == 2 && currentPlayer == 1) {
            // Switch to player 2
            switchToNextPlayer();
        } else if (numPlayers == 2 && currentPlayer == 2) {
            // Show final results
            setState(GameState::FINAL_RESULTS);
        } else {
            // Single player - back to menu
            setState(GameState::MENU);
        }
    }

    if (input->isBackPressed()) {
        setState(GameState::MENU);
    }
}

void Game::updateHighScores() {
    if (input->isSelectPressed() || input->isBackPressed()) {
        setState(GameState::MENU);
    }
}

void Game::updatePlayerSwitch() {
    if (input->isSelectPressed()) {
        resetCurrentPlayer();
        setState(GameState::PLAYING);
    }

    if (input->isBackPressed()) {
        setState(GameState::MENU);
    }
}

void Game::updateFinalResults() {
    if (input->isSelectPressed() || input->isBackPressed()) {
        setState(GameState::MENU);
    }
}

// === State Render Methods ===

void Game::renderMenu() {
    renderer->drawMenu(menu->getSelectedOption());
}

void Game::renderPlayerSelect() {
    renderer->drawPlayerSelect(menu->getSelectedOption());
}

void Game::renderEnterInitials() {
    renderer->drawInitialsEntry(menu->getInitials(), currentPlayer, menu->getCursorPosition());
}

void Game::renderPlaying() {
    renderer->drawGrid();
    renderer->drawSnake(snake);
    renderer->drawFood(food);
    renderer->drawScore(score, highScores->getTopScore());

    if (numPlayers == 2) {
        renderer->drawPlayerInfo(players[currentPlayer - 1].initials, currentPlayer);
    }
}

void Game::renderPaused() {
    renderer->drawPauseScreen();
}

void Game::renderGameOver() {
    renderer->drawGameOver(score, newHighScore);
}

void Game::renderHighScores() {
    renderer->drawHighScores(highScores->getScores());
}

void Game::renderPlayerSwitch() {
    renderer->drawPlayerSwitch(currentPlayer, players[currentPlayer - 1].initials);
}

void Game::renderFinalResults() {
    renderer->drawFinalResults(
        players[0].initials, players[0].score,
        players[1].initials, players[1].score
    );
}

// === Game Logic Helpers ===

void Game::startNewGame() {
    currentPlayer = 1;
    resetCurrentPlayer();
    setState(GameState::PLAYING);
}

void Game::resetCurrentPlayer() {
    snake.reset();
    food.spawn(snake);

    score = 0;
    gameSpeed = Constants::INITIAL_GAME_SPEED;
    moveTimer = 0;
    foodEaten = 0;
    newHighScore = false;
}

void Game::handleGameOver() {
    snake.setAlive(false);

    // Store score for current player
    players[currentPlayer - 1].score = score;

    // Check and record high score
    newHighScore = highScores->isHighScore(score);
    if (newHighScore) {
        highScores->addScore(players[currentPlayer - 1].initials, score);
    }

    setState(GameState::GAME_OVER);
}

void Game::switchToNextPlayer() {
    currentPlayer = 2;
    setState(GameState::PLAYER_SWITCH);
}
