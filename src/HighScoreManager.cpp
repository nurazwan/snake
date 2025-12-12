#include "HighScoreManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdio>

HighScoreManager::HighScoreManager()
    : filepath(Constants::HIGHSCORE_PATH) {
}

bool HighScoreManager::load() {
    scores.clear();

    std::ifstream file(filepath);
    if (!file.is_open()) {
        // File doesn't exist yet - that's okay
        return true;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();
    file.close();

    return parseJson(json);
}

bool HighScoreManager::save() {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        printf("Error: Could not open high scores file for writing\n");
        return false;
    }

    file << toJson();
    file.close();
    return true;
}

int HighScoreManager::addScore(const std::string& initials, int score) {
    if (score <= 0) return 0;

    HighScoreEntry entry;
    entry.initials = initials.substr(0, Constants::INITIALS_LENGTH);
    entry.score = score;
    entry.date = getCurrentDate();

    // Pad initials if too short
    while (entry.initials.length() < Constants::INITIALS_LENGTH) {
        entry.initials += ' ';
    }

    scores.push_back(entry);
    sortAndTrim();

    // Find the rank of this score
    for (size_t i = 0; i < scores.size(); ++i) {
        if (scores[i].score == score && scores[i].initials == entry.initials &&
            scores[i].date == entry.date) {
            save();
            return static_cast<int>(i + 1);
        }
    }

    return 0; // Didn't make the high score list
}

bool HighScoreManager::isHighScore(int score) const {
    if (score <= 0) return false;

    if (scores.size() < Constants::MAX_HIGH_SCORES) {
        return true;
    }

    // Check if score beats the lowest high score
    return score > scores.back().score;
}

int HighScoreManager::getTopScore() const {
    if (scores.empty()) return 0;
    return scores.front().score;
}

void HighScoreManager::clear() {
    scores.clear();
    save();
}

void HighScoreManager::sortAndTrim() {
    // Sort by score (highest first)
    std::sort(scores.begin(), scores.end(),
              [](const HighScoreEntry& a, const HighScoreEntry& b) {
                  return a.score > b.score;
              });

    // Keep only top scores
    if (scores.size() > Constants::MAX_HIGH_SCORES) {
        scores.resize(Constants::MAX_HIGH_SCORES);
    }
}

std::string HighScoreManager::getCurrentDate() const {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
             timeinfo->tm_year + 1900,
             timeinfo->tm_mon + 1,
             timeinfo->tm_mday);

    return std::string(buffer);
}

bool HighScoreManager::parseJson(const std::string& json) {
    // Simple JSON parser for our specific format
    // Format: {"scores":[{"initials":"AAA","score":100,"date":"2025-12-12"},...]}

    size_t scoresStart = json.find("\"scores\"");
    if (scoresStart == std::string::npos) return true; // Empty or invalid

    size_t arrayStart = json.find('[', scoresStart);
    size_t arrayEnd = json.find(']', arrayStart);
    if (arrayStart == std::string::npos || arrayEnd == std::string::npos) {
        return true;
    }

    std::string arrayContent = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

    // Parse each entry
    size_t pos = 0;
    while (pos < arrayContent.length()) {
        size_t objStart = arrayContent.find('{', pos);
        size_t objEnd = arrayContent.find('}', objStart);
        if (objStart == std::string::npos || objEnd == std::string::npos) break;

        std::string objContent = arrayContent.substr(objStart + 1, objEnd - objStart - 1);

        HighScoreEntry entry;

        // Parse initials
        size_t initialsPos = objContent.find("\"initials\"");
        if (initialsPos != std::string::npos) {
            size_t valueStart = objContent.find('\"', initialsPos + 10);
            size_t valueEnd = objContent.find('\"', valueStart + 1);
            if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                entry.initials = objContent.substr(valueStart + 1, valueEnd - valueStart - 1);
            }
        }

        // Parse score
        size_t scorePos = objContent.find("\"score\"");
        if (scorePos != std::string::npos) {
            size_t valueStart = objContent.find(':', scorePos);
            if (valueStart != std::string::npos) {
                entry.score = std::stoi(objContent.substr(valueStart + 1));
            }
        }

        // Parse date
        size_t datePos = objContent.find("\"date\"");
        if (datePos != std::string::npos) {
            size_t valueStart = objContent.find('\"', datePos + 6);
            size_t valueEnd = objContent.find('\"', valueStart + 1);
            if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                entry.date = objContent.substr(valueStart + 1, valueEnd - valueStart - 1);
            }
        }

        if (!entry.initials.empty() && entry.score > 0) {
            scores.push_back(entry);
        }

        pos = objEnd + 1;
    }

    sortAndTrim();
    return true;
}

std::string HighScoreManager::toJson() const {
    std::stringstream ss;
    ss << "{\n  \"scores\": [\n";

    for (size_t i = 0; i < scores.size(); ++i) {
        ss << "    {\"initials\": \"" << scores[i].initials
           << "\", \"score\": " << scores[i].score
           << ", \"date\": \"" << scores[i].date << "\"}";

        if (i < scores.size() - 1) {
            ss << ",";
        }
        ss << "\n";
    }

    ss << "  ]\n}";
    return ss.str();
}
