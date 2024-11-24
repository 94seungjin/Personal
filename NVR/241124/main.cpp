#include <thread>
#include <iostream>
#include "EdgeCamera.h"
#include "ClientReceiver.h"
#include "Storage.h"
#include "Server.h"
#include "Viewer.h"

int main()
{
    try
    {
        // 1. ClientReceiver 역할: 데이터를 수신하고 Storage에 저장
        video::Storage storage; // Storage 객체
        std::thread clientReceiverThread([&storage]() {
            const int port = 5000; // Edge와 통신할 포트
            ReceiveAndStoreFrames(storage, port);
        });

        // ClientReceiver가 준비될 시간을 줌
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // 2. EdgeCamera 역할: Edge에서 데이터를 캡처하고 전송
        std::thread edgeCameraThread([]() {
            const std::string serverAddress = "127.0.0.1"; // NVR(Client) 주소
            const int serverPort = 5000; // NVR(Client) 포트
            StartCameraAndSendFrames(serverAddress, serverPort);
        });

        // 3. Server 역할: Storage에서 데이터를 읽어 Viewer에 전송
        std::thread serverThread([&storage]() {
            const int serverPort = 6000; // Viewer와 통신할 포트
            Server server(serverPort, storage);
            server.Run();
        });

        // 4. Viewer 역할: Server에서 데이터를 수신하고 재생
		std::thread viewerThread([]() {
	    const std::string serverAddress = "127.0.0.1"; // Server 주소
    	const int serverPort = 6000; // Server 포트

	    std::this_thread::sleep_for(std::chrono::seconds(2)); // 데이터 준비 시간 제공

    	Viewer viewer(serverAddress, serverPort);
    	viewer.Run(0); // 0번 프레임부터 시작
});

        // 각 스레드를 병렬로 실행
        edgeCameraThread.join();
        clientReceiverThread.join();
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

