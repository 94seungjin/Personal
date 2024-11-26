#include "Edge.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

// Edge의 간단한 데이터 전송 구현
void sendFramesToClient(int numFrames) {
    std::srand(std::time(0)); // 난수 초기화

    for (int i = 1; i <= numFrames; ++i) {
        std::cout << "[Edge] Sending frame " << i << " to Client\n";
        // 임의의 대기 시간 시뮬레이션
        std::this_thread::sleep_for(std::chrono::milliseconds(100 + std::rand() % 100));
    }
}

