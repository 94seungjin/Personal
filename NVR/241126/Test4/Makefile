CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/Storage.cpp \
          $(SRC_DIR)/Edge.cpp \
          $(SRC_DIR)/Client.cpp \
          $(SRC_DIR)/Server.cpp \
          $(SRC_DIR)/Viewer.cpp

OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
EXECUTABLE = $(BIN_DIR)/app

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean

