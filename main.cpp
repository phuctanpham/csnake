#include "snake.h"
#include "labyrinth.h"

int main() {
    Difficulty difficulty = chooseLabyrinthDifficulty();
    Labyrinth labyrinth(difficulty);
    Game game(&labyrinth);
    game.run();
    return 0;
}
