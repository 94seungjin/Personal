# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/opencv4

# JSON library path (change if installed elsewhere)
JSON_LIB = /path/to/nlohmann/json.hpp

# OpenCV Libraries
OPENCV_LIBS = `pkg-config --cflags --libs opencv4`

# Directories and files
SRC_DIR = .
OBJ_DIR = ./obj
BIN_DIR = ./bin
TARGET = $(BIN_DIR)/storage_test

# Source files
SRCS = $(SRC_DIR)/Storage.cpp $(SRC_DIR)/main.cpp
OBJS = $(OBJ_DIR)/Storage.o $(OBJ_DIR)/main.o

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCV_LIBS)

# Object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(JSON_LIB) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

