# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread

# OpenCV flags
OPENCV_LIBS = `pkg-config --cflags --libs opencv4`

# Source files and executable
SRCS = EdgeCamera.cpp ClientReceiver.cpp Server.cpp Storage.cpp Viewer.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = test_app

# Rules
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC) $(OPENCV_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(OPENCV_LIBS)

clean:
	rm -f $(OBJS) $(EXEC)

