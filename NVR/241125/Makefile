# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Include paths for OpenCV and nlohmann-json
INCLUDES = -I/usr/include/opencv4 -I/usr/include/nlohmann

# Libraries for OpenCV
OPENCV_LIBS = `pkg-config --libs opencv4`

# Target executable
TARGET = storage_test

# Source files
SRCS = main.cpp Storage.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(INCLUDES) $(OPENCV_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

