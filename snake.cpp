#include <cstdlib>
#include <iostream>
#include <ctime>
#include "snake.h"
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif

using namespace std;

namespace {
#ifdef _WIN32
class TerminalModeGuard {
public:
    TerminalModeGuard() {}
    ~TerminalModeGuard() {}
};
#else
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
#endif

#ifdef _WIN32
bool keyAvailable() {
    return _kbhit() != 0;
}

int readKey() {
    return _getch();
}


void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        system("cls");
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        system("cls");
        return;
    }

    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD count;
    COORD homeCoords = {0, 0};

    FillConsoleOutputCharacter(hConsole, ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hConsole, homeCoords);
}
#else
bool keyAvailable() {
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(STDIN_FILENO, &readSet);

    return select(STDIN_FILENO + 1, &readSet, NULL, NULL, &timeout) > 0;
}

int readKey() {
    char key;
    ssize_t bytesRead = read(STDIN_FILENO, &key, 1);
    if (bytesRead == 1) {
        return static_cast<unsigned char>(key);
    }
    return -1;
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
#endif

bool decodeDirectionKey(int key, Direction& direction) {
    if (key == 'a' || key == 'A') {
        direction = LEFT;
        return true;
    }
    if (key == 'w' || key == 'W') {
        direction = UP;
        return true;
    }
    if (key == 'd' || key == 'D') {
        direction = RIGHT;
        return true;
    }
    if (key == 'x' || key == 'X') {
        direction = DOWN;
        return true;
    }

    return false;
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

bool isOppositeDirection(Direction dir1, Direction dir2) {
    return (dir1 + 2) % 4 == dir2;
}

Game::Game(Labyrinth* labyrinth) : labyrinth(labyrinth), currentDirection(RIGHT), gameOver(false) {
    food.x = 0;
    food.y = 0;
    srand(static_cast<unsigned int>(time(NULL)));
    spawnFood();
    scoreState.load();
}

void Game::run() {
    TerminalModeGuard terminalModeGuard;

    while (!gameOver) {
        handleInput();
        update();
        clearScreen();
        drawScene();
        cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    clearScreen();
    drawScene();
    drawGameOver();
}

void Game::handleInput() {
    Direction newDirection = currentDirection;

    // Đọc hết tất cả key có sẵn trong buffer
    while (keyAvailable()) {
        int key = readKey();
#ifdef _WIN32
        if (key == 224) {  // Phím mũi tên trên Windows bắt đầu bằng 224
            int arrowCode = readKey();  // Đọc byte thứ hai
            Direction direction = currentDirection;
            if (arrowCode == 72) {
                direction = UP;
            } else if (arrowCode == 80) {
                direction = DOWN;
            } else if (arrowCode == 77) {
                direction = RIGHT;
            } else if (arrowCode == 75) {
                direction = LEFT;
            }
            if (!isOppositeDirection(currentDirection, direction)) {
                newDirection = direction;
            }
        } else if (key == 0) {  // Một số trường hợp Windows dùng 0
            int arrowCode = readKey();
            Direction direction = currentDirection;
            if (arrowCode == 72) {
                direction = UP;
            } else if (arrowCode == 80) {
                direction = DOWN;
            } else if (arrowCode == 77) {
                direction = RIGHT;
            } else if (arrowCode == 75) {
                direction = LEFT;
            }
            if (!isOppositeDirection(currentDirection, direction)) {
                newDirection = direction;
            }
        } else {
            Direction direction = currentDirection;
            if (decodeDirectionKey(key, direction)) {
                if (!isOppositeDirection(currentDirection, direction)) {
                    newDirection = direction;
                }
            }
        }
#else
        if (key == 27) {  // ESC trên Linux
            Direction direction = currentDirection;
            if (tryReadArrowDirection(direction)) {
                if (!isOppositeDirection(currentDirection, direction)) {
                    newDirection = direction;
                }
            }
        } else {
            Direction direction = currentDirection;
            if (decodeDirectionKey(key, direction)) {
                if (!isOppositeDirection(currentDirection, direction)) {
                    newDirection = direction;
                }
            }
        }
#endif
    }

    currentDirection = newDirection;
}

void Game::drawScene() const {
    labyrinth->draw();
    // hiển thị điểm số và điểm cao nhất
     goToXY(1, 1);
    cout << "Score: " << scoreState.current() << " | High Score: " << scoreState.best();
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
        scoreState.addPoints(10);
    }
}

void Game::drawGameOver() const {
    scoreState.save();
    goToXY(labyrinth->left(), labyrinth->bottom() + 2);
    cout << "Game Over. Press Ctrl+C to exit." << endl;
    cout << "Final Score: " << scoreState.current() << endl;
    cout << "High Score: " << scoreState.best() << endl;
    if (scoreState.isNewRecord()) {  // Kiểm tra nếu là kỷ lục mới
        cout << "New High Score!" << endl;
    }
}

#ifdef _WIN32
void goToXY(int column, int line) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        COORD coord;
        coord.X = static_cast<SHORT>(column);
        coord.Y = static_cast<SHORT>(line);
        SetConsoleCursorPosition(hConsole, coord);
    }
}
#else
void goToXY(int column, int line) {
    cout << "\033[" << (line + 1) << ";" << (column + 1) << "H";
}
#endif
