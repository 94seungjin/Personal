#include "Storage.h"
#include "Server.h"
#include "Viewer.h"
#include <thread>
#include <iostream>

int main()
{
    try
    {
        // 1. Storage 초기화 및 데이터 준비
        std::string videoPath = "cctv.mp4";
        video::Storage storage;
        storage.ExtractAndStoreFrames(videoPath);
        std::cout << "Frames extracted and stored successfully." << std::endl;

        // 2. Server 실행 (스레드로 실행)
        int serverPort = 8080;
        viewer::Server server(storage, serverPort);
        std::thread serverThread([&server]() {
            server.Start();
        });

        // 3. Viewer 실행 (스레드로 실행)
        std::string serverIP = "127.0.0.1"; // 로컬 테스트
        viewer::Viewer viewer;
        std::thread viewerThread([&viewer, &serverIP, serverPort]() {
            // Viewer가 서버에서 유효한 프레임 범위를 요청
            int startFrame, endFrame;
            viewer.requestFrameRange(serverIP, serverPort, startFrame, endFrame);

            // 요청 범위를 기반으로 데이터 수신 시작
            viewer.Start(startFrame, endFrame);
        });

        // 4. 스레드 대기
        serverThread.join();
        viewerThread.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

