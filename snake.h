#ifndef SNAKE_H
#define SNAKE_H

#include "labyrinth.h"

enum Direction {
    RIGHT = 0,
    DOWN = 1,
    LEFT = 2,
    UP = 3
};

class Snake {
private:
    static const int MAX_LENGTH = 100;
    Point body[MAX_LENGTH];
    int length;

public:
    Snake();
    void draw() const;
    void move(Direction direction);
    void grow();
    Point head() const;
    bool occupies(int x, int y) const;
    bool hitsBody(int x, int y) const;
    bool hitsBodyExceptTail(int x, int y) const;
    Point nextHead(Direction direction) const;
};

class Game {
private:
    Labyrinth* labyrinth;
    Snake snake;
    Direction currentDirection;
    Point food;
    bool gameOver;

public:
    explicit Game(Labyrinth* labyrinth);
    void run();

private:
    void handleInput();
    void drawScene() const;
    void spawnFood();
    void update();
    void drawGameOver() const;
};

void goToXY(int column, int line);

#endif
