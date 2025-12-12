#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <utility>
#include "Constants.h"

struct Position {
    int x;
    int y;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
public:
    Snake();
    ~Snake() = default;

    // Core methods
    void reset();
    void move();
    void grow();
    void setDirection(Direction newDir);

    // Collision detection
    bool checkWallCollision() const;
    bool checkSelfCollision() const;
    bool checkCollisionAt(const Position& pos) const;

    // Getters
    const Position& getHead() const { return segments.front(); }
    const std::vector<Position>& getSegments() const { return segments; }
    Direction getDirection() const { return direction; }
    int getLength() const { return static_cast<int>(segments.size()); }
    bool isAlive() const { return alive; }

    // Setters
    void setAlive(bool value) { alive = value; }

private:
    std::vector<Position> segments;
    Direction direction;
    Direction nextDirection; // Buffered direction to prevent 180-degree turns
    bool alive;
    bool hasEaten; // Flag to grow on next move
};

#endif // SNAKE_H
