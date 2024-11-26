# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4` -lpthread

# Target and Source Files
TARGET = main
SRCS = main.cpp Storage.cpp
OBJS = $(SRCS:.cpp=.o)

# Default Target
all: $(TARGET)

# Linking Target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Object File Compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean Build Files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Install Dependencies (Optional)
#install_deps:
#	sudo apt-get install -y build-essential libopencv-dev

