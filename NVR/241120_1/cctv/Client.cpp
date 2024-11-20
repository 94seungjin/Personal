#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

#include "Client.h"
#include "Frame.h"

namespace cctv
{
	logger::Logger Client::logger("Client");

	Client::Client(std::string& host, int port)
		: mHost(host)
		, mPort(port)
		, mbClosed(false)
		, mSocketFd(-1)
	{
	}

	Client::~Client()
	{
		Close();
	}

	void Client::Start()
	{
		connectToServer();

		receiveFrameAndSaveToFile();

	}

	/*
	- inet_pton: 문자열 형식의 IP 주소를 네트워크 형식으로 변환하는 함수.
	- htons: 호스트 바이트 순서를 네트워크 바이트 순서로 변환.
	- sockaddr_in: IPv4 주소 정보를 저장하는 구조체.
	- AF_INET: IPv4 인터넷 프로토콜을 의미하는 주소 패밀리.
	*/
	void Client::connectToServer()
	{
		logger.Info("connectToServer() start");

		// 소켓 생성
		mSocketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (mSocketFd < 0)
		{
			logger.Error("socket() fail");
			return;
		}
		logger.Info("socket() success");

		// 서버 주소 설정
		sockaddr_in server_addr = { 0, };
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(mPort); // htons(port)는 호스트 바이트 순서(Host Byte Order)를 네트워크 바이트 순서(Network Byte Order)로 바꿔주는 함수. 바이트 순서가 호스트와 네트워크 간에 다를 수 있으므로, 네트워크에서 사용할 수 있도록 포트를 변환.
		inet_pton(AF_INET, mHost.c_str(), &server_addr.sin_addr);
		logger.Info("sockaddr_in structure setting success");

		// 서버에 연결
		if (connect(mSocketFd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		{
			logger.Error("connect() fail");
			return;
		}
		logger.Info("connect() success");
	}

	void Client::Close()
	{
		if (mbClosed)
		{
			return;
		}

		mbClosed = true;
		close(mSocketFd);
		logger.Info("close(socketFd)");
	}

	void Client::receiveFrameAndSaveToFile()
	{
		char frameBuffer[Frame::FRAME_SIZE];
		int totalBytesReceived = 0;

		std::string filePath = std::string(PROJECT_ROOT) + "/storage/" + mHost;
		logger.Info("filePath: " + filePath);
		FILE* file = fopen(filePath.c_str(), "ab");
		if (!file)
		{
			logger.Error("Failed to open output file");
			return;
		}

		while (true)
		{	
	        int bytesReceived = recv(mSocketFd, frameBuffer + totalBytesReceived, Frame::FRAME_SIZE - totalBytesReceived, 0);
			if (bytesReceived < 0)
			{
				if (errno == EINTR) 
				{
					continue; // 인터럽트 발생 시 재시도
				}
				logger.Error("recv() fail");
				break;
			}
			else if (bytesReceived == 0)
			{
				// 서버가 연결을 종료함
				logger.Info("Connection closed by server");
				break;
			}

			totalBytesReceived += bytesReceived;

			// 전체 데이터를 수신했는지 확인
			if (totalBytesReceived >= Frame::FRAME_SIZE)
			{
				fwrite(frameBuffer, sizeof(char), Frame::FRAME_SIZE, file); // 프레임 데이터를 파일에 저장

				// 다음 프레임 수신을 위해 초기화
				totalBytesReceived = 0;
			}
		}
		
		fclose(file);
	}
}