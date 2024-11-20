CXX = g++
CXXFLAGS = -std=c++17 -I../common -I/usr/include/opencv4 -DPROJECT_ROOT=\"../\"
LDFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgcodecs

SRC = main.cpp Viewer.cpp ../common/log/Logger.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = viewer

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

