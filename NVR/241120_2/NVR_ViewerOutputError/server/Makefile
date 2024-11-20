CC = g++
CFLAGS = -std=c++17 -I../common -I../storage -I/usr/include/opencv4 -DPROJECT_ROOT=\"../\"
LDFLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_highgui
OBJS = main.o Server.o Storage.o ../common/log/Logger.o

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.cpp Server.hpp
	$(CC) $(CFLAGS) -c main.cpp

Server.o: Server.cpp Server.hpp ../storage/Storage.hpp ../common/log/Logger.h
	$(CC) $(CFLAGS) -c Server.cpp

Storage.o: ../storage/Storage.cpp ../storage/Storage.hpp ../common/log/Logger.h
	$(CC) $(CFLAGS) -c ../storage/Storage.cpp

../common/log/Logger.o: ../common/log/Logger.cpp ../common/log/Logger.h
	$(CC) $(CFLAGS) -c ../common/log/Logger.cpp -o ../common/log/Logger.o

clean:
	rm -f $(OBJS) $(TARGET)

