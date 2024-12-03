# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Target executable
TARGET = main

# Source and header files
SOURCES = main.cpp VideoHeader.cpp FrameBody.cpp FrameHeader.cpp FrameQueue.cpp Storage.cpp
HEADERS = VideoHeader.h FrameBody.h FrameHeader.h FrameQueue.h Storage.h

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Build target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJECTS)

