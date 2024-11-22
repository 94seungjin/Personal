# 컴파일러 설정
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/opencv4
LDFLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lpthread

# 실행 파일 이름
TARGET = main

# 소스 파일 및 오브젝트 파일
SRCS = main.cpp Storage.cpp Server.cpp Viewer.cpp
OBJS = $(SRCS:.cpp=.o)

# 기본 빌드 규칙
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 클린 규칙
clean:
	rm -f $(OBJS) $(TARGET)

# 실행 규칙
run: all
	./$(TARGET)

