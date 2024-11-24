#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>

void StartCameraAndSendFrames(const std::string& serverAddress, int serverPort)
{
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
    {
        throw std::runtime_error("Failed to create socket.");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

    if (connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("Failed to connect to server.");
    }

	cv::VideoCapture cap(0, cv::CAP_V4L2); // Raspberry Pi 카메라 강제 초기화
	if (!cap.isOpened())
	{
    	throw std::runtime_error("Failed to open camera.");
	}
	while (true)
	{
    	cv::Mat frame;
	    cap >> frame; // 프레임 캡처

	    if (frame.empty())
	    {
    	    std::cerr << "Captured empty frame, retrying..." << std::endl;
	        usleep(100000); // 100ms 대기 후 재시도
    	    continue;
	    }

	    // 프레임을 JPEG로 압축
	    std::vector<uchar> buffer;
	    std::vector<int> compressionParams = {cv::IMWRITE_JPEG_QUALITY, 90};
	    cv::imencode(".jpg", frame, buffer, compressionParams);

	    // 데이터 전송
	    uint32_t frameSize = htonl(buffer.size());
	    if (send(socketFd, &frameSize, sizeof(frameSize), 0) <= 0 ||
			send(socketFd, buffer.data(), buffer.size(), 0) <= 0)
    	{
        	std::cerr << "Failed to send frame data. Retrying..." << std::endl;
	        usleep(100000); // 100ms 대기 후 재시도
    	    continue;
	    }

    	std::cout << "Sent frame of size: " << buffer.size() << " bytes." << std::endl;
	}

    close(socketFd);
}

