# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Output executable
TARGET = storage

# Source and header files
SRCS = main.cpp \
       VideoHeader.cpp \
       FrameHeader.cpp \
       FrameQueue.cpp \
       Storage.cpp

HEADERS = Serialize.h \
          VideoHeader.h \
          FrameHeader.h \
          FrameQueue.h \
          FrameBody.h \
          Storage.h

OBJS = $(SRCS:.cpp=.o)

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

