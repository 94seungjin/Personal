#include "Viewer.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace viewer
{
    void Viewer::requestFrameRange(const std::string& serverIP, int serverPort, int& startFrame, int& endFrame)
    {
        int sockFd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockFd == -1)
        {
            throw std::runtime_error("Failed to create socket");
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

        if (connect(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        {
            close(sockFd);
            throw std::runtime_error("Failed to connect to server");
        }

        // 요청: 프레임 범위
        uint32_t rangeRequest = htonl(1); // 1: 범위 요청
        send(sockFd, &rangeRequest, sizeof(rangeRequest), 0);

        // 범위 수신
        uint32_t minFrame, maxFrame;
        recv(sockFd, &minFrame, sizeof(minFrame), 0);
        recv(sockFd, &maxFrame, sizeof(maxFrame), 0);

        startFrame = ntohl(minFrame);
        endFrame = ntohl(maxFrame);

        std::cout << "Server provides frames from " << startFrame << " to " << endFrame << std::endl;

        close(sockFd);
    }

    void Viewer::Start(int startFrame, int endFrame)
    {
        int sockFd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockFd == -1)
        {
            throw std::runtime_error("Failed to create socket");
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080); // 서버 포트
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

        if (connect(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        {
            close(sockFd);
            throw std::runtime_error("Failed to connect to server");
        }

        std::cout << "Connected to server at 127.0.0.1:8080" << std::endl;

        // 범위 내 프레임 요청
        for (int frame = startFrame; frame <= endFrame; ++frame)
        {
            receiveFrame(sockFd);
        }

        close(sockFd);
    }

	void Viewer::receiveFrame(int serverSocketFd)
	{
    	// JSON 데이터 크기 수신
	    uint32_t jsonSize;
    	if (recv(serverSocketFd, &jsonSize, sizeof(jsonSize), 0) <= 0)
	    {
    	    throw std::runtime_error("Failed to receive JSON size");
	    }
    	jsonSize = ntohl(jsonSize);

	    // JSON 데이터 수신
	    std::vector<char> jsonData(jsonSize);
	    if (recv(serverSocketFd, jsonData.data(), jsonSize, 0) <= 0)
	    {
	        throw std::runtime_error("Failed to receive JSON data");
	    }

    	// 이미지 데이터 크기 수신
	    uint32_t dataSize;
	    if (recv(serverSocketFd, &dataSize, sizeof(dataSize), 0) <= 0)
	    {
    	    throw std::runtime_error("Failed to receive image size");
	    }
    	dataSize = ntohl(dataSize);

	    // 이미지 데이터 수신
    	std::vector<uchar> imageData(dataSize);
	    if (recv(serverSocketFd, imageData.data(), dataSize, 0) <= 0)
    	{
        	throw std::runtime_error("Failed to receive image data");
	    }

    	// JPEG 데이터를 디코딩하여 Mat로 변환
	    cv::Mat frame = cv::imdecode(imageData, cv::IMREAD_COLOR);
	    if (frame.empty())
	    {
    	    std::cerr << "Failed to decode frame" << std::endl;
        	return;
	    }

    	// OpenCV를 통해 프레임 표시
	    cv::imshow("Viewer", frame);
    	cv::waitKey(30); // 30ms 대기 (프레임 속도 조절)
	}
}
