#include "Viewer.hpp"
#include <thread>
#include <iostream>

int main()
{
    // 서버 IP와 포트 설정
    std::string serverIp = "127.0.0.1";
    int serverPort = 8080;

    // Viewer 인스턴스 생성
    Viewer viewer(serverIp, serverPort);

    // Viewer를 별도 스레드에서 실행
    std::thread viewerThread([&viewer]() {
        viewer.Start();
    });

    // 사용자에게 종료 안내
    std::cout << "Press 'q' in the viewer window to stop." << std::endl;

    // Viewer 실행 대기
    viewerThread.join();

    return 0;
}

