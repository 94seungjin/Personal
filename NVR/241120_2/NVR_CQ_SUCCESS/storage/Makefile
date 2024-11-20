CC = g++
CFLAGS = -std=c++17 -I/usr/include/opencv4
LDFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio

all: test_storage

test_storage: main.o Storage.o
	$(CC) -o test_storage main.o Storage.o $(LDFLAGS)

main.o: main.cpp Storage.hpp
	$(CC) $(CFLAGS) -c main.cpp

Storage.o: Storage.cpp Storage.hpp
	$(CC) $(CFLAGS) -c Storage.cpp

clean:
	rm -f *.o test_storage

