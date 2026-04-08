#include "score.h"

#include <fstream>

using namespace std;

namespace {
int loadHighScore() {
    ifstream file("highscore.txt");
    int hs = 0;
    if (file.is_open()) {
        file >> hs;
        file.close();
    }
    return hs;
}

void saveHighScore(int hs) {
    ofstream file("highscore.txt");
    if (file.is_open()) {
        file << hs;
        file.close();
    }
}
}

ScoreState::ScoreState() : score(0), highScore(0) {
}

int ScoreState::current() const {
    return score;
}

int ScoreState::best() const {
    return highScore;
}

bool ScoreState::isNewRecord() const {
    return score > 0 && score == highScore;
}

void ScoreState::addPoints(int points) {
    score += points;
    if (score > highScore) {
        highScore = score;
    }
}

void ScoreState::load() {
    highScore = loadHighScore();
}

void ScoreState::save() const {
    saveHighScore(highScore);
}
