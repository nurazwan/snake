#include "Renderer.h"
#include <cstdio>
#include <cmath>

Renderer::Renderer()
    : window(nullptr)
    , renderer(nullptr)
    , fontSmall(nullptr)
    , fontMedium(nullptr)
    , fontLarge(nullptr)
    , fontTitle(nullptr)
    , initialized(false)
    , frameCount(0) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init() {
    // Initialize SDL video
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        printf("Error: Could not initialize SDL video: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        printf("Error: Could not initialize SDL_ttf: %s\n", TTF_GetError());
        return false;
    }

    // Create window
    window = SDL_CreateWindow(
        Constants::WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Constants::WINDOW_WIDTH,
        Constants::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Error: Could not create window: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer with vsync
    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("Error: Could not create renderer: %s\n", SDL_GetError());
        return false;
    }

    // Set blend mode for transparency
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Load fonts at different sizes
    fontSmall = TTF_OpenFont(Constants::FONT_PATH, 16);
    fontMedium = TTF_OpenFont(Constants::FONT_PATH, 24);
    fontLarge = TTF_OpenFont(Constants::FONT_PATH, 32);
    fontTitle = TTF_OpenFont(Constants::FONT_PATH, 64);

    // If custom font fails, try system fonts
    if (!fontSmall || !fontMedium || !fontLarge || !fontTitle) {
        printf("Warning: Could not load pixel font, trying system fonts\n");

        // Try common system font paths
        const char* fallbackFonts[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
            "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
            "/System/Library/Fonts/Menlo.ttc",
            "C:\\Windows\\Fonts\\consola.ttf",
            nullptr
        };

        for (int i = 0; fallbackFonts[i] != nullptr; ++i) {
            if (!fontSmall) fontSmall = TTF_OpenFont(fallbackFonts[i], 16);
            if (!fontMedium) fontMedium = TTF_OpenFont(fallbackFonts[i], 24);
            if (!fontLarge) fontLarge = TTF_OpenFont(fallbackFonts[i], 32);
            if (!fontTitle) fontTitle = TTF_OpenFont(fallbackFonts[i], 64);

            if (fontSmall && fontMedium && fontLarge && fontTitle) break;
        }
    }

    if (!fontMedium) {
        printf("Error: Could not load any font\n");
        return false;
    }

    // Use available fonts as fallback
    if (!fontSmall) fontSmall = fontMedium;
    if (!fontLarge) fontLarge = fontMedium;
    if (!fontTitle) fontTitle = fontLarge;

    initialized = true;
    return true;
}

void Renderer::shutdown() {
    if (fontTitle && fontTitle != fontLarge) TTF_CloseFont(fontTitle);
    if (fontLarge && fontLarge != fontMedium) TTF_CloseFont(fontLarge);
    if (fontMedium) TTF_CloseFont(fontMedium);
    if (fontSmall && fontSmall != fontMedium) TTF_CloseFont(fontSmall);

    fontTitle = fontLarge = fontMedium = fontSmall = nullptr;

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    initialized = false;
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer,
        Constants::Colors::BG_R,
        Constants::Colors::BG_G,
        Constants::Colors::BG_B,
        Constants::Colors::BG_A);
    SDL_RenderClear(renderer);
    frameCount++;
}

void Renderer::present() {
    // Optional: Add scanline effect
    // drawScanlines();
    SDL_RenderPresent(renderer);
}

int Renderer::gridToScreenX(int gridX) const {
    return gridX * Constants::CELL_SIZE;
}

int Renderer::gridToScreenY(int gridY) const {
    return gridY * Constants::CELL_SIZE + Constants::GRID_OFFSET_Y;
}

SDL_Color Renderer::makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return {r, g, b, a};
}

void Renderer::drawText(const std::string& text, int x, int y, SDL_Color color,
                        bool centered, int fontSize) {
    if (text.empty()) return;

    TTF_Font* font = fontMedium;
    if (fontSize <= 16) font = fontSmall;
    else if (fontSize >= 48) font = fontTitle;
    else if (fontSize >= 32) font = fontLarge;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect destRect;
    destRect.w = surface->w;
    destRect.h = surface->h;
    destRect.x = centered ? (x - destRect.w / 2) : x;
    destRect.y = y;

    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Renderer::drawRect(int x, int y, int w, int h, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, w, h};

    if (filled) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void Renderer::drawRectWithOutline(int x, int y, int w, int h,
                                   SDL_Color fillColor, SDL_Color outlineColor) {
    // Draw fill
    drawRect(x, y, w, h, fillColor, true);

    // Draw outline (2 pixels thick)
    SDL_SetRenderDrawColor(renderer, outlineColor.r, outlineColor.g,
                           outlineColor.b, outlineColor.a);

    SDL_Rect outer = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &outer);

    SDL_Rect inner = {x + 1, y + 1, w - 2, h - 2};
    SDL_RenderDrawRect(renderer, &inner);
}

void Renderer::drawGrid() {
    SDL_Color gridColor = makeColor(
        Constants::Colors::GRID_R,
        Constants::Colors::GRID_G,
        Constants::Colors::GRID_B,
        Constants::Colors::GRID_A
    );

    SDL_SetRenderDrawColor(renderer, gridColor.r, gridColor.g,
                           gridColor.b, gridColor.a);

    // Draw vertical lines
    for (int x = 0; x <= Constants::GRID_WIDTH; ++x) {
        int screenX = gridToScreenX(x);
        SDL_RenderDrawLine(renderer, screenX, Constants::GRID_OFFSET_Y,
                           screenX, Constants::WINDOW_HEIGHT);
    }

    // Draw horizontal lines
    for (int y = 0; y <= Constants::GRID_HEIGHT; ++y) {
        int screenY = gridToScreenY(y);
        SDL_RenderDrawLine(renderer, 0, screenY,
                           Constants::WINDOW_WIDTH, screenY);
    }
}

void Renderer::drawSnake(const Snake& snake) {
    const auto& segments = snake.getSegments();
    if (segments.empty()) return;

    SDL_Color headColor = makeColor(
        Constants::Colors::HEAD_R,
        Constants::Colors::HEAD_G,
        Constants::Colors::HEAD_B,
        Constants::Colors::HEAD_A
    );

    SDL_Color bodyColor = makeColor(
        Constants::Colors::BODY_R,
        Constants::Colors::BODY_G,
        Constants::Colors::BODY_B,
        Constants::Colors::BODY_A
    );

    SDL_Color outlineColor = makeColor(
        Constants::Colors::OUTLINE_R,
        Constants::Colors::OUTLINE_G,
        Constants::Colors::OUTLINE_B,
        Constants::Colors::OUTLINE_A
    );

    // Draw body segments (back to front for proper overlap)
    for (size_t i = segments.size() - 1; i > 0; --i) {
        int x = gridToScreenX(segments[i].x) + 1;
        int y = gridToScreenY(segments[i].y) + 1;

        // Gradient from body color to darker for tail
        float fadeRatio = static_cast<float>(i) / segments.size();
        SDL_Color segColor = {
            static_cast<Uint8>(bodyColor.r * (0.6f + 0.4f * (1.0f - fadeRatio))),
            static_cast<Uint8>(bodyColor.g * (0.6f + 0.4f * (1.0f - fadeRatio))),
            static_cast<Uint8>(bodyColor.b * (0.6f + 0.4f * (1.0f - fadeRatio))),
            255
        };

        drawRectWithOutline(x, y, Constants::CELL_SIZE - 2, Constants::CELL_SIZE - 2,
                           segColor, outlineColor);
    }

    // Draw head
    int headX = gridToScreenX(segments[0].x) + 1;
    int headY = gridToScreenY(segments[0].y) + 1;
    drawRectWithOutline(headX, headY, Constants::CELL_SIZE - 2, Constants::CELL_SIZE - 2,
                       headColor, outlineColor);

    // Draw eyes on head
    Direction dir = snake.getDirection();
    int eyeSize = 4;

    SDL_Color eyeColor = makeColor(255, 255, 255, 255);
    SDL_Color pupilColor = makeColor(0, 0, 0, 255);

    int eyeX1 = headX + 3, eyeY1 = headY + 3;
    int eyeX2 = headX + Constants::CELL_SIZE - 8, eyeY2 = headY + 3;

    switch (dir) {
        case Direction::UP:
            eyeY1 = eyeY2 = headY + 3;
            break;
        case Direction::DOWN:
            eyeY1 = eyeY2 = headY + Constants::CELL_SIZE - 8;
            break;
        case Direction::LEFT:
            eyeX1 = eyeX2 = headX + 3;
            eyeY2 = headY + Constants::CELL_SIZE - 8;
            break;
        case Direction::RIGHT:
            eyeX1 = eyeX2 = headX + Constants::CELL_SIZE - 8;
            eyeY2 = headY + Constants::CELL_SIZE - 8;
            break;
        default:
            break;
    }

    drawRect(eyeX1, eyeY1, eyeSize, eyeSize, eyeColor, true);
    drawRect(eyeX2, eyeY2, eyeSize, eyeSize, eyeColor, true);

    // Draw pupils
    drawRect(eyeX1 + 1, eyeY1 + 1, 2, 2, pupilColor, true);
    drawRect(eyeX2 + 1, eyeY2 + 1, 2, 2, pupilColor, true);
}

void Renderer::drawFood(const Food& food) {
    const Position& pos = food.getPosition();
    int x = gridToScreenX(pos.x);
    int y = gridToScreenY(pos.y);

    // Pulsing effect
    float pulse = food.getPulseValue();
    int expansion = static_cast<int>(pulse * 3);

    // Draw glow
    SDL_Color glowColor = makeColor(
        Constants::Colors::FOOD_GLOW_R,
        Constants::Colors::FOOD_GLOW_G,
        Constants::Colors::FOOD_GLOW_B,
        static_cast<uint8_t>(Constants::Colors::FOOD_GLOW_A * pulse)
    );
    drawRect(x - expansion - 2, y - expansion - 2,
             Constants::CELL_SIZE + expansion * 2 + 4,
             Constants::CELL_SIZE + expansion * 2 + 4,
             glowColor, true);

    // Draw food
    SDL_Color foodColor = makeColor(
        Constants::Colors::FOOD_R,
        Constants::Colors::FOOD_G,
        Constants::Colors::FOOD_B,
        Constants::Colors::FOOD_A
    );

    drawRect(x + 2 - expansion, y + 2 - expansion,
             Constants::CELL_SIZE - 4 + expansion * 2,
             Constants::CELL_SIZE - 4 + expansion * 2,
             foodColor, true);

    // Draw highlight
    SDL_Color highlightColor = makeColor(255, 150, 150, 200);
    drawRect(x + 4, y + 4, 4, 4, highlightColor, true);
}

void Renderer::drawScore(int score, int highScore) {
    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    SDL_Color highlightColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    // Draw score on left
    drawText("SCORE", 20, 10, textColor, false, 16);
    drawText(std::to_string(score), 20, 28, highlightColor, false, 24);

    // Draw high score on right
    drawText("HIGH SCORE", Constants::WINDOW_WIDTH - 120, 10, textColor, false, 16);
    drawText(std::to_string(highScore), Constants::WINDOW_WIDTH - 120, 28, highlightColor, false, 24);

    // Draw separator line
    SDL_Color lineColor = makeColor(50, 50, 80, 255);
    SDL_SetRenderDrawColor(renderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    SDL_RenderDrawLine(renderer, 0, Constants::GRID_OFFSET_Y - 2,
                       Constants::WINDOW_WIDTH, Constants::GRID_OFFSET_Y - 2);
}

void Renderer::drawPlayerInfo(const std::string& initials, int playerNum) {
    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    std::string info = "P" + std::to_string(playerNum) + ": " + initials;
    drawText(info, Constants::WINDOW_WIDTH / 2, 10, textColor, true, 16);
}

void Renderer::drawTitleScreen() {
    SDL_Color titleColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    // Animate title
    int yOffset = static_cast<int>(std::sin(frameCount * 0.05) * 5);

    drawText("SNAKE", Constants::WINDOW_WIDTH / 2, 150 + yOffset, titleColor, true, 64);
    drawText("Press ENTER or A to Start", Constants::WINDOW_WIDTH / 2, 350, textColor, true, 24);
    drawText("Press ESC or B to Quit", Constants::WINDOW_WIDTH / 2, 400, textColor, true, 16);
}

void Renderer::drawMenu(int selectedOption) {
    SDL_Color titleColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    SDL_Color selectColor = makeColor(
        Constants::Colors::SELECT_R,
        Constants::Colors::SELECT_G,
        Constants::Colors::SELECT_B,
        Constants::Colors::SELECT_A
    );

    // Title
    drawText("SNAKE", Constants::WINDOW_WIDTH / 2, 80, titleColor, true, 64);

    // Menu options
    const char* options[] = {"1 PLAYER", "2 PLAYERS", "HIGH SCORES", "QUIT"};
    int numOptions = 4;
    int startY = 220;
    int spacing = 60;

    for (int i = 0; i < numOptions; ++i) {
        int y = startY + i * spacing;
        SDL_Color color = (i == selectedOption) ? selectColor : textColor;

        if (i == selectedOption) {
            // Draw selection indicator
            drawText(">", Constants::WINDOW_WIDTH / 2 - 100, y, color, false, 32);
            drawText("<", Constants::WINDOW_WIDTH / 2 + 80, y, color, false, 32);
        }

        drawText(options[i], Constants::WINDOW_WIDTH / 2, y, color, true, 32);
    }

    // Instructions
    drawText("Use W/S or D-Pad to select", Constants::WINDOW_WIDTH / 2, 500, textColor, true, 16);
    drawText("Press ENTER or A to confirm", Constants::WINDOW_WIDTH / 2, 525, textColor, true, 16);
}

void Renderer::drawPlayerSelect(int selectedOption) {
    SDL_Color titleColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    SDL_Color selectColor = makeColor(
        Constants::Colors::SELECT_R,
        Constants::Colors::SELECT_G,
        Constants::Colors::SELECT_B,
        Constants::Colors::SELECT_A
    );

    drawText("HOW MANY PLAYERS?", Constants::WINDOW_WIDTH / 2, 150, titleColor, true, 32);

    const char* options[] = {"1 PLAYER", "2 PLAYERS"};
    int startY = 280;
    int spacing = 80;

    for (int i = 0; i < 2; ++i) {
        int y = startY + i * spacing;
        SDL_Color color = (i == selectedOption) ? selectColor : textColor;

        if (i == selectedOption) {
            drawText(">", Constants::WINDOW_WIDTH / 2 - 100, y, color, false, 32);
        }

        drawText(options[i], Constants::WINDOW_WIDTH / 2, y, color, true, 32);
    }

    drawText("Press ESC or B to go back", Constants::WINDOW_WIDTH / 2, 500, textColor, true, 16);
}

void Renderer::drawInitialsEntry(const std::string& initials, int playerNum, int cursorPos) {
    SDL_Color titleColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    std::string title = "PLAYER " + std::to_string(playerNum) + " - ENTER INITIALS";
    drawText(title, Constants::WINDOW_WIDTH / 2, 150, titleColor, true, 24);

    // Draw initials boxes
    int boxWidth = 60;
    int boxHeight = 80;
    int spacing = 20;
    int startX = Constants::WINDOW_WIDTH / 2 - (boxWidth * 3 + spacing * 2) / 2;
    int y = 250;

    for (int i = 0; i < 3; ++i) {
        int x = startX + i * (boxWidth + spacing);

        // Draw box
        SDL_Color boxColor = (i == cursorPos)
            ? makeColor(60, 60, 100, 255)
            : makeColor(30, 30, 50, 255);
        drawRect(x, y, boxWidth, boxHeight, boxColor, true);

        // Draw border
        SDL_Color borderColor = (i == cursorPos) ? titleColor : textColor;
        drawRect(x, y, boxWidth, boxHeight, borderColor, false);

        // Draw letter
        if (i < static_cast<int>(initials.length())) {
            std::string letter(1, initials[i]);
            drawText(letter, x + boxWidth / 2, y + 15, textColor, true, 48);
        }

        // Draw cursor
        if (i == cursorPos && (frameCount / 30) % 2 == 0) {
            drawRect(x + 10, y + boxHeight - 15, boxWidth - 20, 4, titleColor, true);
        }
    }

    drawText("Type your initials (3 letters)", Constants::WINDOW_WIDTH / 2, 400, textColor, true, 20);
    drawText("Press ENTER when done", Constants::WINDOW_WIDTH / 2, 430, textColor, true, 16);
    drawText("Press BACKSPACE to delete", Constants::WINDOW_WIDTH / 2, 455, textColor, true, 16);
}

void Renderer::drawPauseScreen() {
    // Semi-transparent overlay
    SDL_Color overlayColor = makeColor(0, 0, 0, 180);
    drawRect(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, overlayColor, true);

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    SDL_Color highlightColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    drawText("PAUSED", Constants::WINDOW_WIDTH / 2, 250, highlightColor, true, 48);
    drawText("Press P or START to continue", Constants::WINDOW_WIDTH / 2, 350, textColor, true, 20);
    drawText("Press ESC or B to quit", Constants::WINDOW_WIDTH / 2, 385, textColor, true, 16);
}

void Renderer::drawGameOver(int score, bool isHighScore) {
    SDL_Color titleColor = makeColor(255, 50, 50, 255); // Red for game over

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    SDL_Color highlightColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    drawText("GAME OVER", Constants::WINDOW_WIDTH / 2, 180, titleColor, true, 48);

    drawText("SCORE", Constants::WINDOW_WIDTH / 2, 280, textColor, true, 20);
    drawText(std::to_string(score), Constants::WINDOW_WIDTH / 2, 310, highlightColor, true, 48);

    if (isHighScore) {
        drawText("NEW HIGH SCORE!", Constants::WINDOW_WIDTH / 2, 380, highlightColor, true, 24);
    }

    drawText("Press ENTER or A to continue", Constants::WINDOW_WIDTH / 2, 480, textColor, true, 20);
}

void Renderer::drawHighScores(const std::vector<HighScoreEntry>& scores) {
    SDL_Color titleColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    SDL_Color goldColor = makeColor(255, 215, 0, 255);
    SDL_Color silverColor = makeColor(192, 192, 192, 255);
    SDL_Color bronzeColor = makeColor(205, 127, 50, 255);

    drawText("HIGH SCORES", Constants::WINDOW_WIDTH / 2, 50, titleColor, true, 36);

    // Column headers
    int rankX = 100;
    int initialsX = 200;
    int scoreX = 400;
    int dateX = 600;
    int headerY = 110;

    drawText("RANK", rankX, headerY, textColor, false, 16);
    drawText("NAME", initialsX, headerY, textColor, false, 16);
    drawText("SCORE", scoreX, headerY, textColor, false, 16);
    drawText("DATE", dateX, headerY, textColor, false, 16);

    // Draw scores
    int startY = 150;
    int spacing = 40;

    for (size_t i = 0; i < scores.size() && i < 10; ++i) {
        int y = startY + static_cast<int>(i) * spacing;

        // Color based on rank
        SDL_Color rankColor = textColor;
        if (i == 0) rankColor = goldColor;
        else if (i == 1) rankColor = silverColor;
        else if (i == 2) rankColor = bronzeColor;

        std::string rank = std::to_string(i + 1) + ".";
        drawText(rank, rankX, y, rankColor, false, 20);
        drawText(scores[i].initials, initialsX, y, textColor, false, 20);
        drawText(std::to_string(scores[i].score), scoreX, y, textColor, false, 20);
        drawText(scores[i].date, dateX, y, textColor, false, 16);
    }

    if (scores.empty()) {
        drawText("No high scores yet!", Constants::WINDOW_WIDTH / 2, 300, textColor, true, 24);
    }

    drawText("Press ENTER or B to go back", Constants::WINDOW_WIDTH / 2, 550, textColor, true, 16);
}

void Renderer::drawPlayerSwitch(int playerNum, const std::string& initials) {
    SDL_Color titleColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    std::string title = "PLAYER " + std::to_string(playerNum);
    std::string name = "[ " + initials + " ]";

    drawText(title, Constants::WINDOW_WIDTH / 2, 200, titleColor, true, 48);
    drawText(name, Constants::WINDOW_WIDTH / 2, 280, textColor, true, 32);
    drawText("GET READY!", Constants::WINDOW_WIDTH / 2, 360, titleColor, true, 32);
    drawText("Press ENTER or A to start", Constants::WINDOW_WIDTH / 2, 450, textColor, true, 20);
}

void Renderer::drawFinalResults(const std::string& p1Initials, int p1Score,
                                const std::string& p2Initials, int p2Score) {
    SDL_Color titleColor = makeColor(
        Constants::Colors::HIGHLIGHT_R,
        Constants::Colors::HIGHLIGHT_G,
        Constants::Colors::HIGHLIGHT_B,
        Constants::Colors::HIGHLIGHT_A
    );

    SDL_Color textColor = makeColor(
        Constants::Colors::TEXT_R,
        Constants::Colors::TEXT_G,
        Constants::Colors::TEXT_B,
        Constants::Colors::TEXT_A
    );

    SDL_Color goldColor = makeColor(255, 215, 0, 255);

    drawText("FINAL RESULTS", Constants::WINDOW_WIDTH / 2, 100, titleColor, true, 36);

    // Player 1 score
    int p1Y = 200;
    drawText("PLAYER 1: " + p1Initials, Constants::WINDOW_WIDTH / 2, p1Y, textColor, true, 24);
    drawText(std::to_string(p1Score), Constants::WINDOW_WIDTH / 2, p1Y + 40, textColor, true, 36);

    // Player 2 score
    int p2Y = 320;
    drawText("PLAYER 2: " + p2Initials, Constants::WINDOW_WIDTH / 2, p2Y, textColor, true, 24);
    drawText(std::to_string(p2Score), Constants::WINDOW_WIDTH / 2, p2Y + 40, textColor, true, 36);

    // Winner announcement
    int winnerY = 440;
    if (p1Score > p2Score) {
        drawText("WINNER: " + p1Initials + "!", Constants::WINDOW_WIDTH / 2, winnerY, goldColor, true, 32);
    } else if (p2Score > p1Score) {
        drawText("WINNER: " + p2Initials + "!", Constants::WINDOW_WIDTH / 2, winnerY, goldColor, true, 32);
    } else {
        drawText("IT'S A TIE!", Constants::WINDOW_WIDTH / 2, winnerY, goldColor, true, 32);
    }

    drawText("Press ENTER or A to continue", Constants::WINDOW_WIDTH / 2, 530, textColor, true, 20);
}

void Renderer::drawScanlines() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 30);
    for (int y = 0; y < Constants::WINDOW_HEIGHT; y += 3) {
        SDL_RenderDrawLine(renderer, 0, y, Constants::WINDOW_WIDTH, y);
    }
}
