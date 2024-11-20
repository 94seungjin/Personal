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
		, storage(100, metadataDir, rawdataDir)
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

	    try	
		{
    	    char buffer[1024];
       	 	// 클라이언트로부터 요청받을 타임스탬프
       	 	ssize_t bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        	if (bytesRead <= 0)
			{
            	logger.Warning("Client disconnected before sending timestamp");
            	return;
       		}
        	buffer[bytesRead] = '\0';
	        std::string requestedTimestamp(buffer);
    	    logger.Info("Requested timestamp: " + requestedTimestamp);

        	// 메타데이터 읽기
	        Metadata metadata = storage.loadMetadata(requestedTimestamp);
	        logger.Info("Loaded metadata: " + metadata.timestamp);

    	    // 메타데이터 전송
        	std::string metaPacket = "Metadata:" + metadata.info;
	        send(socketFd, metaPacket.c_str(), metaPacket.size(), 0);

    	    // 원본 데이터 읽기
        	RawData rawdata = storage.loadRawData(requestedTimestamp);
	        logger.Info("Loaded raw data: " + rawdata.timestamp);

    	    // 원본 데이터 전송 (바이너리)
       		 send(socketFd, rawdata.data.data(), rawdata.data.size(), 0);

        	logger.Info("Streaming completed for timestamp: " + requestedTimestamp);
    	}
		catch (const std::exception& e)
		{
        	logger.Error("Error during streaming: " + std::string(e.what()));
    	}
	}
}
