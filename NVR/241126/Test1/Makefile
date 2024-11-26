# 컴파일러 및 플래그 설정
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# 디렉토리 설정
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin

# 소스 파일 및 대상 파일
SRCS = $(SRC_DIR)/Storage.cpp $(SRC_DIR)/main.cpp
OBJS = $(BUILD_DIR)/Storage.o $(BUILD_DIR)/main.o
TARGET = $(BIN_DIR)/app

# 기본 규칙
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# 개별 소스 파일 컴파일 규칙
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 정리
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

