#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "Storage.h"

void ReceiveAndStoreFrames(video::Storage& storage, int port)
{

	int frameNumber = 0;	// 프레임 번호 초기화

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        throw std::runtime_error("Failed to create server socket.");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("Failed to bind server socket.");
    }

    if (listen(serverSocket, 5) < 0)
    {
        throw std::runtime_error("Failed to listen on server socket.");
    }

    std::cout << "Client is listening on port " << port << std::endl;

    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0)
        {
            std::cerr << "Failed to accept connection." << std::endl;
            continue;
        }

        std::cout << "Connected to Edge device." << std::endl;
        int frameNumber = 0; // 프레임 번호 초기화
        while (true)
        {
            // 데이터 수신
            uint32_t frameSize;
            if (recv(clientSocket, &frameSize, sizeof(frameSize), 0) <= 0)
            {
                std::cerr << "Failed to receive frame size." << std::endl;
                break;
            }
            frameSize = ntohl(frameSize);
        
            std::vector<uint8_t> frameData(frameSize);
            if (recv(clientSocket, frameData.data(), frameSize, 0) <= 0)
            {
                std::cerr << "Failed to receive frame data." << std::endl;
                break;
            }

            // Storage에 저장
            video::FrameHeader header =
            {
                "timestamp_placeholder",  // 타임스탬프 생성 필요
                frameNumber++,            // 프레임 번호 증가 (경고 제거)
                640, 480,                 // 해상도
                "JPEG"                    // 압축 형식
            };

            video::FrameBody body = {frameData};
            storage.StoreFrame(header, body);

            std::cout << "Stored a frame of size: " << frameSize << " bytes." << std::endl;
        }

        close(clientSocket);
    }
    close(serverSocket);
}


