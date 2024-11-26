#include <iostream>
#include <thread>
#include "Storage.h"
#include "Edge.h"
#include "Client.h"
#include "Server.h"
#include "Viewer.h"

int main() {
    // Storage 초기화
    video::Storage storage("data_storage", 5);

    // Client, Server, Viewer 생성
    Client client(storage);
    Server server(storage);
    Viewer viewer;

    // Edge 데이터 전송 및 처리
    std::thread edgeThread(sendFramesToClient, 10);
    edgeThread.join();

    // Client가 데이터를 받아 저장
    for (int i = 1; i <= 10; ++i) {
        client.receiveAndStoreFrame(i);
    }

    // Server가 Storage에서 데이터를 가져와 Viewer로 전송
    for (int i = 1; i <= 10; ++i) {
        server.fetchAndSendToViewer(i);
        viewer.displayFrame(i);
    }

    // Storage 상태 출력
    std::cout << "Storage 상태:\n" << storage.GetStatus() << "\n";

    return 0;
}

