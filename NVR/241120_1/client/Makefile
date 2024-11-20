# 컴파일러 설정
CXX = g++
CXXFLAGS = -std=c++17 -I. -I../common -I/usr/include/opencv4 -DPROJECT_ROOT=\"../\"
LDFLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_highgui

# 소스 파일 및 실행 파일
SRC = Viewer.cpp main.cpp ../common/log/Logger.cpp
TARGET = viewer.out

# 기본 빌드 규칙
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# 정리 규칙
clean:
	rm -f $(TARGET)

