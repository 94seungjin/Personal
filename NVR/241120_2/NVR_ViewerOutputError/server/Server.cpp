#include "Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <opencv2/opencv.hpp>

logger::Logger Server::logger("Server");

Server::Server(int port, const std::string& metadataDir, const std::string& rawdataDir)
    : serverPort(port), storage(100, metadataDir, rawdataDir), serverSocketFd(-1) {}

void Server::setupServer()
{
    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFd == -1)
    {
        logger.Error("Failed to create socket");
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocketFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        logger.Error("Failed to bind socket");
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(serverSocketFd, 3) == -1)
    {
        logger.Error("Failed to listen on socket");
        throw std::runtime_error("Failed to listen on socket");
    }

    logger.Info("Server setup complete and listening on port " + std::to_string(serverPort));
}

void Server::Start()
{
    setupServer();

    while (true)
	{
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        logger.Info("Waiting for client connection...");

        int clientSocketFd = accept(serverSocketFd, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocketFd == -1)
		{
            logger.Warning("Failed to accept client connection");
            continue;
        }

        handleClient(clientSocketFd);
        close(clientSocketFd);
        logger.Info("Client disconnected");
    }
}

void Server::handleClient(int clientSocketFd)
{
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0)
	{
        logger.Warning("Failed to receive timestamp from client");
        return;
    }
    buffer[bytesRead] = '\0';
    std::string timestamp(buffer);
    logger.Info("Received request for timestamp: " + timestamp);

    try
	{
        Metadata metadata = storage.loadMetadata(timestamp);
        sendMetadata(clientSocketFd, metadata);

        RawData rawData = storage.loadRawData(timestamp);
        sendRawData(clientSocketFd, rawData);
    }
	catch (const std::exception& e)
	{
        logger.Error("Error while processing client request: " + std::string(e.what()));
    }
}

void Server::sendMetadata(int clientSocketFd, const Metadata& metadata)
{
    std::string metadataStr = "Metadata: Timestamp=" + metadata.timestamp +
                              ", FrameWidth=" + std::to_string(metadata.frameWidth) +
                              ", FrameHeight=" + std::to_string(metadata.frameHeight) +
                              ", TotalFrames=" + std::to_string(metadata.totalFrames);

    ssize_t bytesSent = send(clientSocketFd, metadataStr.c_str(), metadataStr.size(), 0);
    if (bytesSent <= 0)
	{
        logger.Warning("Failed to send metadata to client");
    }
	else
	{
        logger.Info("Metadata sent to client");
    }
}

void Server::sendRawData(int clientSocketFd, const RawData& rawData)
{
    // RawData가 올바른지 확인
    cv::Mat testFrame = cv::imdecode(rawData.data, cv::IMREAD_COLOR);
    if (testFrame.empty())
    {
        logger.Error("RawData is corrupted, cannot decode.");
        return;
    }
    logger.Info("RawData is valid, proceeding with transmission.");

	// 전송 전에 데이터 크기 로깅
    logger.Info("Sending raw data size: " + std::to_string(rawData.data.size()));

    // RawData 전송
    ssize_t bytesSent = send(clientSocketFd, rawData.data.data(), rawData.data.size(), 0);
    if (bytesSent <= 0)
    {
        logger.Warning("Failed to send raw data to client");
    }
    else
    {
        logger.Info("Raw data sent to client, size: " + std::to_string(bytesSent));
    }
}

