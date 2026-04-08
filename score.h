#ifndef SCORE_H
#define SCORE_H

class ScoreState {
private:
    int score;
    int highScore;

public:
    ScoreState();

    int current() const;
    int best() const;
    bool isNewRecord() const;

    void addPoints(int points);
    void load();
    void save() const;
};

#endif
