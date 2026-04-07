#include <cstdlib>
#include <iostream>
#include <ctime>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include "snake.h"
#include <fstream>

using namespace std;

namespace {
class TerminalModeGuard {
private:
    termios originalTermios;
    bool isConfigured;

public:
    TerminalModeGuard() : isConfigured(false) {
        if (tcgetattr(STDIN_FILENO, &originalTermios) == -1) {
            return;
        }

        termios rawMode = originalTermios;
        rawMode.c_lflag &= static_cast<unsigned int>(~(ICANON | ECHO));
        rawMode.c_cc[VMIN] = 0;
        rawMode.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &rawMode) == 0) {
            isConfigured = true;
        }
    }

    ~TerminalModeGuard() {
        if (isConfigured) {
            tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
        }
    }
};

bool keyAvailable() {
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(STDIN_FILENO, &readSet);

    return select(STDIN_FILENO + 1, &readSet, NULL, NULL, &timeout) > 0;
}

char readKey() {
    char key;
    ssize_t bytesRead = read(STDIN_FILENO, &key, 1);
    if (bytesRead == 1) {
        return key;
    }
    return '\0';
}

bool decodeDirectionKey(char key, Direction& direction) {
    if (key == 'a') {
        direction = LEFT;
        return true;
    }
    if (key == 'w') {
        direction = UP;
        return true;
    }
    if (key == 'd') {
        direction = RIGHT;
        return true;
    }
    if (key == 'x') {
        direction = DOWN;
        return true;
    }

    return false;
}

bool tryReadArrowDirection(Direction& direction) {
    char sequence[2];

    if (read(STDIN_FILENO, &sequence[0], 1) != 1) {
        return false;
    }

    if (sequence[0] != '[') {
        return false;
    }

    if (read(STDIN_FILENO, &sequence[1], 1) != 1) {
        return false;
    }

    if (sequence[1] == 'A') {
        direction = UP;
        return true;
    }
    if (sequence[1] == 'B') {
        direction = DOWN;
        return true;
    }
    if (sequence[1] == 'C') {
        direction = RIGHT;
        return true;
    }
    if (sequence[1] == 'D') {
        direction = LEFT;
        return true;
    }

    return false;
}

void clearScreen() {
    cout << "\033[2J\033[H";
}
}

Snake::Snake() : length(3) {
    body[0].x = 10;
    body[0].y = 10;
    body[1].x = 9;
    body[1].y = 10;
    body[2].x = 8;
    body[2].y = 10;
}

void Snake::grow() {
    if (length < MAX_LENGTH) {
        ++length;
    }
}

Point Snake::head() const {
    return body[0];
}

bool Snake::occupies(int x, int y) const {
    for (int i = 0; i < length; ++i) {
        if (body[i].x == x && body[i].y == y) {
            return true;
        }
    }
    return false;
}

bool Snake::hitsBody(int x, int y) const {
    for (int i = 1; i < length; ++i) {
        if (body[i].x == x && body[i].y == y) {
            return true;
        }
    }
    return false;
}

bool Snake::hitsBodyExceptTail(int x, int y) const {
    if (length <= 1) {
        return false;
    }

    for (int i = 1; i < length - 1; ++i) {
        if (body[i].x == x && body[i].y == y) {
            return true;
        }
    }
    return false;
}

Point Snake::nextHead(Direction direction) const {
    Point next = body[0];

    if (direction == RIGHT) {
        next.x += 1;
    }
    if (direction == DOWN) {
        next.y += 1;
    }
    if (direction == LEFT) {
        next.x -= 1;
    }
    if (direction == UP) {
        next.y -= 1;
    }

    return next;
}

void Snake::draw() const {
    for (int i = 0; i < length; ++i) {
        goToXY(body[i].x, body[i].y);
        cout << "X";
    }
}

void Snake::move(Direction direction) {
    for (int i = length - 1; i > 0; --i) {
        body[i] = body[i - 1];
    }

    if (direction == RIGHT) {
        body[0].x += 1;
    }
    if (direction == DOWN) {
        body[0].y += 1;
    }
    if (direction == LEFT) {
        body[0].x -= 1;
    }
    if (direction == UP) {
        body[0].y -= 1;
    }
}

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

bool isOppositeDirection(Direction dir1, Direction dir2) {
    return (dir1 + 2) % 4 == dir2;
}

Game::Game(Labyrinth* labyrinth) : labyrinth(labyrinth), currentDirection(RIGHT), gameOver(false) {
    food.x = 0;
    food.y = 0;
    srand(static_cast<unsigned int>(time(NULL)));
    spawnFood();
    score = 0;
    highScore = loadHighScore();
}

void Game::run() {
    TerminalModeGuard terminalModeGuard;

    while (!gameOver) {
        handleInput();
        update();
        clearScreen();
        drawScene();
        cout.flush();
        usleep(300000);
    }

    clearScreen();
    drawScene();
    drawGameOver();
}

void Game::handleInput() {
    if (!keyAvailable()) {
        return;
    }

    char key = readKey();
    if (key == 27) {
        Direction direction = currentDirection;
        if (tryReadArrowDirection(direction)) {
            if (!isOppositeDirection(currentDirection, direction)) {
                currentDirection = direction;
            }
        }
        return;
    }

    Direction direction = currentDirection;
    if (decodeDirectionKey(key, direction)) {
        if (!isOppositeDirection(currentDirection, direction)) {
            currentDirection = direction;
        }
    }
}

void Game::drawScene() const {
    labyrinth->draw();
    // hiển thị điểm số và điểm cao nhất
     goToXY(1, 1);
    cout << "Score: " << score << " | High Score: " << highScore;
    // vẽ mồi và rắn
    goToXY(food.x, food.y);
    cout << "*";
    snake.draw();
}

void Game::spawnFood() {
    food = labyrinth->randomEmptyCell(snake);
}

void Game::update() {
    Point nextHead = snake.nextHead(currentDirection);

    if (!labyrinth->isInside(nextHead.x, nextHead.y) || labyrinth->isBarrier(nextHead.x, nextHead.y) || snake.hitsBodyExceptTail(nextHead.x, nextHead.y)) {
        gameOver = true;
        return;
    }

    snake.move(currentDirection);

    Point snakeHead = snake.head();
    if (snakeHead.x == food.x && snakeHead.y == food.y) {
        snake.grow();
        spawnFood();
        score += 10;
        if (score > highScore) {
            highScore = score;
        }
    }
}

void Game::drawGameOver() const {
    saveHighScore(highScore);
    goToXY(labyrinth->left(), labyrinth->bottom() + 2);
    cout << "Game Over. Press Ctrl+C to exit." << endl;
    cout << "Final Score: " << score << endl;
    cout << "High Score: " << highScore << endl;
    if (score == highScore && score > 0) {  // Kiểm tra nếu là kỷ lục mới
        cout << "New High Score!" << endl;
    }
}

void goToXY(int column, int line) {
    cout << "\033[" << (line + 1) << ";" << (column + 1) << "H";
}
