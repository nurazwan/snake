#ifndef FOOD_H
#define FOOD_H

#include "Constants.h"
#include "Snake.h"
#include <random>

class Food {
public:
    Food();
    ~Food() = default;

    // Spawn food at a random position not occupied by the snake
    void spawn(const Snake& snake);

    // Get current position
    const Position& getPosition() const { return position; }

    // Get pulse value for animation (0.0 to 1.0)
    float getPulseValue() const;

    // Update animation
    void update();

private:
    Position position;
    std::mt19937 rng;
    int pulseTimer;
};

#endif // FOOD_H
