# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS =

# Directories
SRCDIR = .
BUILDDIR = build
BINDIR = bin
TARGET = $(BINDIR)/app

# Source and object files
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/Storage.cpp
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Create directories
$(shell mkdir -p $(BUILDDIR) $(BINDIR))

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build and binary directories
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Run the application
run: all
	./$(TARGET)

.PHONY: all clean run

