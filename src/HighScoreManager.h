#ifndef HIGHSCOREMANAGER_H
#define HIGHSCOREMANAGER_H

#include <vector>
#include <string>
#include "Constants.h"

struct HighScoreEntry {
    std::string initials;
    int score;
    std::string date;

    bool operator>(const HighScoreEntry& other) const {
        return score > other.score;
    }
};

class HighScoreManager {
public:
    HighScoreManager();
    ~HighScoreManager() = default;

    // Load high scores from file
    bool load();

    // Save high scores to file
    bool save();

    // Add a new score (returns rank 1-10, or 0 if not a high score)
    int addScore(const std::string& initials, int score);

    // Check if a score qualifies as a high score
    bool isHighScore(int score) const;

    // Get all high scores
    const std::vector<HighScoreEntry>& getScores() const { return scores; }

    // Get the highest score
    int getTopScore() const;

    // Clear all high scores
    void clear();

private:
    // Parse JSON manually (lightweight, no external dependencies)
    bool parseJson(const std::string& json);

    // Generate JSON string
    std::string toJson() const;

    // Get current date string (YYYY-MM-DD)
    std::string getCurrentDate() const;

    // Sort and trim scores
    void sortAndTrim();

    std::vector<HighScoreEntry> scores;
    std::string filepath;
};

#endif // HIGHSCOREMANAGER_H
