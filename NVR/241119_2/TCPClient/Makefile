# 컴파일러 설정
CXX = g++
CXXFLAGS = -std=c++17 `pkg-config --cflags opencv4` -pthread -Iinc
LDFLAGS = `pkg-config --libs opencv4`

# 폴더 설정
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# 실행 파일 이름
TARGET = $(BIN_DIR)/viewer

# 소스 파일 및 오브젝트 파일
SRCS = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

# 기본 타겟
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

# 개별 파일 컴파일
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# obj 및 bin 디렉토리 생성
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# 정리
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

