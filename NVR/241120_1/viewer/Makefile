# 컴파일러 설정
CXX = g++
CXXFLAGS = -std=c++17 -I../ -I../common/log -I../storage -DPROJECT_ROOT=\"../\"

# 소스 파일 설정
SRC = Server.cpp main.cpp ../common/log/Logger.cpp ../storage/Storage.cpp
TARGET = main.out

# 빌드 규칙
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# 정리 규칙
clean:
	rm -f $(TARGET)

