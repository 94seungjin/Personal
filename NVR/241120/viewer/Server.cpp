#include <algorithm>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <thread>

#include "viewer/Server.h"
#include "cctv/Frame.h"
#include "test/fixture/Fixture.h"

namespace viewer
{
	logger::Logger Server::logger("Server");

	Server::Server(int port, const std::string& metadataDir, const std::string& rawdataDir)
		: mPort(port)
		, mServerSocketFd(-1)
		, storage(metadataDir, rawdataDir)
	{
	}

	void Server::Start() 
	{
		setupServer();
		
		while (true)
		{
			sockaddr_in clientAddr{};
			socklen_t clientAddrLen = sizeof(clientAddr);
			logger.Info("Waiting for client connection...");

			int clientSocketFd = accept(mServerSocketFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
			if (clientSocketFd == -1) {
				logger.Error("accept() failed");
				continue;
			}

			char clientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			logger.Info("Client connected from IP: " + std::string(clientIP));


			streaming(clientSocketFd);


			close(clientSocketFd); // 클라이언트 소켓 닫기
			logger.Info("Client disconnected");
		}
	}

	void Server::setupServer()
	{
		// server socket
		mServerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (mServerSocketFd == -1) {
			logger.Error("socket() fail");
		}
		logger.Info("socket() success");

		sockaddr_in serverAddr{};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(mPort);
		logger.Info("server sockaddr_in structure setting success");

		// bind
		if (bind(mServerSocketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
		{
			logger.Error("bind() fail");
			close(mServerSocketFd);
		}
		logger.Info("bind() success");

		// listen
		if (listen(mServerSocketFd, Server::BACK_LOG) == -1) 
		{
			logger.Error("listen() fail");
			close(mServerSocketFd);
		}
		logger.Info("listen() success");

		logger.Info("listening port: " + std::to_string(mPort));
	}

	void Server::streaming(int socketFd)
	{
		logger.Info("Streaming started");

	    // 읽어올 데이터 (임의의 타임스탬프 사용, 실제 구현에서는 클라이언트 요청 기반)
    	std::string timestamp = "20231120_123000";

	    try
		{
	        // 메타데이터 읽기
        	Metadata metadata = storage.loadMetadata(timestamp);
	        logger.Info("Metadata loaded: " + metadata.timestamp + ", " + metadata.cameraId + ", " + metadata.info);

    	    // 원본 데이터 읽기
    	    RawData rawdata = storage.loadRawData(timestamp);
        	logger.Info("RawData loaded: " + std::to_string(rawdata.data.size()) + " bytes");

        	// 메타데이터 전송
        	int metadataBytesSent = send(socketFd, metadata.info.c_str(), metadata.info.size(), 0);
	        if (metadataBytesSent <= 0)
			{
            	logger.Error("Failed to send metadata");
	            return;
			}
			// 원본 데이터 전송 (프레임 단위로)
			const size_t frameSize = cctv::Frame::FRAME_SIZE;
			size_t totalBytes = rawdata.data.size();
			size_t sentBytes = 0;

			while (sentBytes < totalBytes)
			{
            	size_t bytesToSend = std::min(frameSize, totalBytes - sentBytes);
            	int bytesSent = send(socketFd, rawdata.data.data() + sentBytes, bytesToSend, 0);
	            if (bytesSent <= 0)
				{
                	logger.Error("Failed to send rawdata or client disconnected");
	                return;
				}
				sentBytes += bytesSent;
				}
				logger.Info("Streaming completed for timestamp: " + timestamp);
			}
		catch (const std::exception& e)
		{
			logger.Error("Error during streaming: " + std::string(e.what()));
		}
	}
}
