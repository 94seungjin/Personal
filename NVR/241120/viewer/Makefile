CXX = g++
CXXFLAGS = -std=c++17 -I/home/sj/Desktop/Personal/NVR/241120 -DPROJECT_ROOT=\"/home/sj/Desktop/Personal/NVR/241120\"
SRC = /home/sj/Desktop/Personal/NVR/241120/common/log/Logger.cpp \
      /home/sj/Desktop/Personal/NVR/241120/viewer/main.cpp \
      /home/sj/Desktop/Personal/NVR/241120/viewer/Server.cpp \
      /home/sj/Desktop/Personal/NVR/241120/test/fixture/Fixture.cpp \
      /home/sj/Desktop/Personal/NVR/241120/storage/Storage.cpp
TARGET = main.out

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

