#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include "Constants.h"
#include "Snake.h"
#include "Food.h"
#include "HighScoreManager.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialize renderer
    bool init();

    // Shutdown renderer
    void shutdown();

    // Clear screen
    void clear();

    // Present rendered frame
    void present();

    // Draw game elements
    void drawGrid();
    void drawSnake(const Snake& snake);
    void drawFood(const Food& food);
    void drawScore(int score, int highScore);
    void drawPlayerInfo(const std::string& initials, int playerNum);

    // Draw UI screens
    void drawTitleScreen();
    void drawMenu(int selectedOption);
    void drawPlayerSelect(int selectedOption);
    void drawInitialsEntry(const std::string& initials, int playerNum, int cursorPos);
    void drawPauseScreen();
    void drawGameOver(int score, bool isHighScore);
    void drawHighScores(const std::vector<HighScoreEntry>& scores);
    void drawPlayerSwitch(int playerNum, const std::string& initials);
    void drawFinalResults(const std::string& p1Initials, int p1Score,
                          const std::string& p2Initials, int p2Score);

    // Get window for event handling
    SDL_Window* getWindow() const { return window; }

private:
    // Helper methods
    void drawText(const std::string& text, int x, int y, SDL_Color color,
                  bool centered = false, int fontSize = 24);
    void drawRect(int x, int y, int w, int h, SDL_Color color, bool filled = true);
    void drawRectWithOutline(int x, int y, int w, int h,
                             SDL_Color fillColor, SDL_Color outlineColor);
    SDL_Color makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    // Grid to screen coordinates
    int gridToScreenX(int gridX) const;
    int gridToScreenY(int gridY) const;

    // Draw scanline effect for retro look
    void drawScanlines();

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* fontSmall;
    TTF_Font* fontMedium;
    TTF_Font* fontLarge;
    TTF_Font* fontTitle;

    bool initialized;
    int frameCount; // For animations
};

#endif // RENDERER_H
