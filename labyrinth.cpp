#include "labyrinth.h"
#include "snake.h"

#include <cstdlib>
#include <iostream>
using namespace std;

namespace {
struct BarrierSegment {
    int x;
    int y;
};

const BarrierSegment HARD_BARRIERS[] = {
    {12, 6}, {13, 6}, {14, 6}, {15, 6}, {16, 6},
    {20, 10}, {20, 11}, {20, 12}, {20, 13},
    {28, 14}, {29, 14}, {30, 14}, {31, 14}
};

const int HARD_BARRIERS_COUNT = sizeof(HARD_BARRIERS) / sizeof(HARD_BARRIERS[0]);

const BarrierSegment HELL_BARRIERS[] = {
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {14, 5}, {15, 5},
    {18, 8}, {18, 9}, {18, 10}, {18, 11}, {18, 12},
    {24, 6}, {24, 7}, {24, 8}, {24, 9},
    {30, 15}, {31, 15}, {32, 15}, {33, 15}, {34, 15},
    {38, 10}, {39, 10}, {40, 10}, {41, 10},
    {46, 18}, {46, 19}, {46, 20},
    {52, 12}, {53, 12}, {54, 12}, {55, 12}, {56, 12}
};

const int HELL_BARRIERS_COUNT = sizeof(HELL_BARRIERS) / sizeof(HELL_BARRIERS[0]);
}

Labyrinth::Labyrinth(Difficulty selectedDifficulty)
    : difficulty(selectedDifficulty), minX(2), minY(2), maxX(35), maxY(20) {
    if (difficulty == DifficultyHard) {
        maxX = 45;
        maxY = 24;
    } else if (difficulty == DifficultyHell) {
        maxX = 60;
        maxY = 28;
    }
}

void Labyrinth::draw() const {
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            if (x == minX || x == maxX || y == minY || y == maxY || isBarrier(x, y)) {
                goToXY(x, y);
                cout << "+";
            }
        }
    }
}

bool Labyrinth::isInside(int x, int y) const {
    return x > minX && x < maxX && y > minY && y < maxY;
}

bool Labyrinth::isBarrier(int x, int y) const {
    if (difficulty == DifficultyEasy) {
        return false;
    }

    if (difficulty == DifficultyHard) {
        for (int i = 0; i < HARD_BARRIERS_COUNT; ++i) {
            if (HARD_BARRIERS[i].x == x && HARD_BARRIERS[i].y == y) {
                return true;
            }
        }
        return false;
    }

    for (int i = 0; i < HELL_BARRIERS_COUNT; ++i) {
        if (HELL_BARRIERS[i].x == x && HELL_BARRIERS[i].y == y) {
            return true;
        }
    }

    return false;
}

Point Labyrinth::randomEmptyCell(const Snake& snake) const {
    Point cell;
    do {
        cell.x = rand() % (maxX - minX - 1) + minX + 1;
        cell.y = rand() % (maxY - minY - 1) + minY + 1;
    } while (isBarrier(cell.x, cell.y) || snake.occupies(cell.x, cell.y));

    return cell;
}

int Labyrinth::left() const {
    return minX;
}

int Labyrinth::top() const {
    return minY;
}

int Labyrinth::right() const {
    return maxX;
}

int Labyrinth::bottom() const {
    return maxY;
}

Difficulty Labyrinth::getDifficulty() const {
    return difficulty;
}
