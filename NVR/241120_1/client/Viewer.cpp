#include "Viewer.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <opencv2/opencv.hpp>

logger::Logger Viewer::logger("Viewer");

Viewer::Viewer(const std::string& serverIP, int port)
    : serverIP(serverIP), port(port), socketFd(-1) {}

Viewer::~Viewer()
{
    if (socketFd != -1)
	{
        close(socketFd);
        logger.Info("Socket closed");
    }
}

void Viewer::Start()
{
    logger.Info("Viewer started");
    connectToServer();
    receiveAndProcessData();
}

void Viewer::connectToServer()
{
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
	{
        logger.Error("Failed to create socket");
        throw std::runtime_error("Failed to create socket");
    }
    logger.Info("Socket created");

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(socketFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
        logger.Error("Failed to connect to server");
        throw std::runtime_error("Failed to connect to server");
    }

    logger.Info("Connected to server at " + serverIP + ":" + std::to_string(port));
}

void Viewer::receiveAndProcessData()
{
    char buffer[1024];
    logger.Info("Start receiving data");

    while (true)
	{
        ssize_t bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0)
		{
            logger.Warning("Disconnected from server");
            break;
        }

        buffer[bytesRead] = '\0';  // Null-terminate for processing as a string
        std::string receivedData(buffer, bytesRead);

        // 메타데이터와 원본 데이터 처리 분기
        if (receivedData.find("Metadata:") == 0)
		{
            logger.Info("Received metadata");
            processMetadata(receivedData);
        }
		else
		{
            logger.Info("Received raw data");
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
    logger.Info("Processing raw data");

    // OpenCV로 프레임 디코딩 및 재생
    cv::Mat frame = cv::imdecode(rawData, cv::IMREAD_COLOR);
    if (frame.empty()) {
        logger.Error("Failed to decode frame");
        return;
    }

    cv::imshow("Video", frame);
    cv::waitKey(30);  // 대기 시간 조정
    logger.Info("Frame displayed");
}
