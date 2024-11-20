#include "Viewer.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp>

logger::Logger Viewer::logger("Viewer");

Viewer::Viewer(const std::string& serverIP, int serverPort)
    : serverIP(serverIP), serverPort(serverPort), clientSocketFd(-1) {}

void Viewer::connectToServer() 
{
    clientSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFd == -1)
	{
        logger.Error("Failed to create socket");
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocketFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
	{
        logger.Error("Failed to connect to server");
        throw std::runtime_error("Failed to connect to server");
    }

    logger.Info("Connected to server at " + serverIP + ":" + std::to_string(serverPort));
}

void Viewer::Start()
{
    connectToServer();
    receiveAndProcessData();
}

void Viewer::receiveAndProcessData()
{
    logger.Info("Start receiving data");

    while (true)
	{
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocketFd, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0)
		{
            logger.Warning("Disconnected from server");
            break;
        }

        buffer[bytesRead] = '\0';
        std::string receivedData(buffer, bytesRead);

        if (receivedData.find("Metadata:") == 0)
		{
            processMetadata(receivedData.substr(9));  // "Metadata:" 이후 처리
        }
		else
		{
            processRawData(std::vector<uint8_t>(buffer, buffer + bytesRead));
        }
    }
}

void Viewer::processMetadata(const std::string& metadata)
{
    logger.Info("Processing metadata: " + metadata);
    std::cout << "Metadata received: " << metadata << std::endl;
}

void Viewer::processRawData(const std::vector<uint8_t>& rawData)
{
    logger.Info("Processing raw data, size: " + std::to_string(rawData.size()));

    if (rawData.empty()) {
        logger.Error("Raw data is empty, skipping frame.");
        return;
    }

    // 디코딩 시도
    cv::Mat frame = cv::imdecode(rawData, cv::IMREAD_COLOR);
    if (frame.empty()) {
        logger.Error("Failed to decode frame, skipping.");
        return;
    }

    logger.Info("Frame decoded successfully, displaying frame.");
    cv::imshow("Video", frame);
    cv::waitKey(30);  // 대기 시간 조정
}

