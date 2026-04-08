CXX ?= clang++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

SRC = main.cpp snake.cpp labyrinth.cpp score.cpp
TARGET = snake
TARGET_EXE = snake.exe

.PHONY: all exe clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

exe: $(TARGET_EXE)

$(TARGET_EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET_EXE)

clean:
	rm -f $(TARGET) $(TARGET_EXE)