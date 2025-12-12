#include "Food.h"
#include <cmath>

Food::Food() : position{0, 0}, pulseTimer(0) {
    // Initialize random number generator with random device
    std::random_device rd;
    rng.seed(rd());
}

void Food::spawn(const Snake& snake) {
    std::uniform_int_distribution<int> distX(0, Constants::GRID_WIDTH - 1);
    std::uniform_int_distribution<int> distY(0, Constants::GRID_HEIGHT - 1);

    // Keep generating positions until we find one not on the snake
    int attempts = 0;
    const int maxAttempts = 1000; // Safety limit

    do {
        position.x = distX(rng);
        position.y = distY(rng);
        attempts++;
    } while (snake.checkCollisionAt(position) && attempts < maxAttempts);

    // Reset pulse animation
    pulseTimer = 0;
}

float Food::getPulseValue() const {
    // Create a smooth pulsing effect using sine wave
    // pulseTimer increments each frame, creating oscillation
    float phase = static_cast<float>(pulseTimer) * 0.1f;
    return (std::sin(phase) + 1.0f) / 2.0f; // Normalize to 0.0 - 1.0
}

void Food::update() {
    pulseTimer++;
    // Keep timer from overflowing
    if (pulseTimer > 1000) {
        pulseTimer = 0;
    }
}
