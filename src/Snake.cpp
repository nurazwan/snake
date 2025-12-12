#include "Snake.h"

Snake::Snake() {
    reset();
}

void Snake::reset() {
    segments.clear();

    // Start in the middle of the grid
    int startX = Constants::GRID_WIDTH / 2;
    int startY = Constants::GRID_HEIGHT / 2;

    // Create initial snake (head + body segments going left)
    for (int i = 0; i < Constants::INITIAL_SNAKE_LENGTH; ++i) {
        segments.push_back({startX - i, startY});
    }

    direction = Direction::RIGHT;
    nextDirection = Direction::RIGHT;
    alive = true;
    hasEaten = false;
}

void Snake::setDirection(Direction newDir) {
    // Prevent 180-degree turns
    if (newDir == Direction::NONE) return;

    bool canChange = true;

    switch (newDir) {
        case Direction::UP:
            canChange = (direction != Direction::DOWN);
            break;
        case Direction::DOWN:
            canChange = (direction != Direction::UP);
            break;
        case Direction::LEFT:
            canChange = (direction != Direction::RIGHT);
            break;
        case Direction::RIGHT:
            canChange = (direction != Direction::LEFT);
            break;
        default:
            canChange = false;
            break;
    }

    if (canChange) {
        nextDirection = newDir;
    }
}

void Snake::move() {
    if (!alive || segments.empty()) return;

    // Apply buffered direction
    direction = nextDirection;

    // Calculate new head position
    Position newHead = segments.front();

    switch (direction) {
        case Direction::UP:
            newHead.y--;
            break;
        case Direction::DOWN:
            newHead.y++;
            break;
        case Direction::LEFT:
            newHead.x--;
            break;
        case Direction::RIGHT:
            newHead.x++;
            break;
        default:
            break;
    }

    // Add new head at the front
    segments.insert(segments.begin(), newHead);

    // Remove tail unless we just ate
    if (hasEaten) {
        hasEaten = false;
    } else {
        segments.pop_back();
    }
}

void Snake::grow() {
    hasEaten = true;
}

bool Snake::checkWallCollision() const {
    if (segments.empty()) return false;

    const Position& head = segments.front();
    return (head.x < 0 || head.x >= Constants::GRID_WIDTH ||
            head.y < 0 || head.y >= Constants::GRID_HEIGHT);
}

bool Snake::checkSelfCollision() const {
    if (segments.size() < 2) return false;

    const Position& head = segments.front();

    // Check if head collides with any body segment
    for (size_t i = 1; i < segments.size(); ++i) {
        if (head == segments[i]) {
            return true;
        }
    }
    return false;
}

bool Snake::checkCollisionAt(const Position& pos) const {
    for (const auto& segment : segments) {
        if (segment == pos) {
            return true;
        }
    }
    return false;
}
