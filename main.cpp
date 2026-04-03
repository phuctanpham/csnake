#include "snake.h"
#include "labyrinth.cpp"
//#include "labyrinth.h"
#include <iostream>

using namespace std;

namespace {
Difficulty chooseDifficulty() {
    int choice = 0;

    cout << "Choose mode:\n";
    cout << "1. Easy\n";
    cout << "2. Hard\n";
    cout << "3. Hell\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 2) {
        return DifficultyHard;
    }

    if (choice == 3) {
        return DifficultyHell;
    }

    return DifficultyEasy;
}
}

int main() {
    Difficulty difficulty = chooseDifficulty();
    Labyrinth labyrinth(difficulty);
    Game game(&labyrinth);
    game.run();
    return 0;
}
