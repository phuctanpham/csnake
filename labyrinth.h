#ifndef LABYRINTH_H
#define LABYRINTH_H

struct Point {
    int x;
    int y;
};

class Snake;

enum Difficulty {
    DifficultyEasy = 1,
    DifficultyHard = 2,
    DifficultyHell = 3
};

Difficulty chooseLabyrinthDifficulty();

class Labyrinth {
private:
    Difficulty difficulty;
    int minX;
    int minY;
    int maxX;
    int maxY;

public:
    explicit Labyrinth(Difficulty difficulty);

    void draw(int score) const;
    bool isInside(int x, int y) const;
    bool isBarrier(int x, int y, int score) const;
    Point randomEmptyCell(const Snake& snake, int score) const;

    int left() const;
    int top() const;
    int right() const;
    int bottom() const;
    Difficulty getDifficulty() const;
};

#endif
