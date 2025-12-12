#include <SDL2/SDL.h>
#include <cstdio>
#include <memory>
#include "Game.h"

int main(int argc, char* argv[]) {
    // Suppress unused parameter warnings
    (void)argc;
    (void)argv;

    printf("=================================\n");
    printf("       SNAKE GAME v1.0\n");
    printf("=================================\n\n");

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("Error: Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    printf("SDL initialized successfully\n");

    // Create and run game
    {
        std::unique_ptr<Game> game = std::make_unique<Game>();

        if (!game->init()) {
            printf("Error: Failed to initialize game\n");
            SDL_Quit();
            return 1;
        }

        printf("Starting game loop...\n\n");
        game->run();

        printf("\nShutting down...\n");
        game->shutdown();
    }

    // Cleanup SDL
    SDL_Quit();

    printf("Game closed successfully. Thanks for playing!\n");
    return 0;
}
