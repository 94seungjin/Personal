#include "Server.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

namespace viewer
{
    Server::Server(video::Storage& storage, int port)
        : mStorage(storage), mPort(port), mServerSocketFd(-1)
    {
    }

    void Server::Start()
    {
        setupServer();

        while (true)
        {
            sockaddr_in clientAddr{};
            socklen_t clientAddrLen = sizeof(clientAddr);
            std::cout << "Waiting for client connection..." << std::endl;

            int clientSocketFd = accept(mServerSocketFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocketFd == -1)
            {
                std::cerr << "accept() failed" << std::endl;
                continue;
            }

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
            std::cout << "Client connected from IP: " << clientIP << std::endl;

            sendFramesInRange(clientSocketFd, 1, 10);

            close(clientSocketFd); // 클라이언트 소켓 닫기
            std::cout << "Client disconnected" << std::endl;
        }
    }

    void Server::setupServer()
    {
        mServerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (mServerSocketFd == -1)
        {
            throw std::runtime_error("Failed to create socket");
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(mPort);

        if (bind(mServerSocketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        {
            close(mServerSocketFd);
            throw std::runtime_error("Failed to bind socket");
        }

        if (listen(mServerSocketFd, 5) == -1)
        {
            close(mServerSocketFd);
            throw std::runtime_error("Failed to listen on socket");
        }

        std::cout << "Server is listening on port " << mPort << std::endl;
    }

    void Server::sendFrame(int clientSocketFd, int frameNumber)
    {
        try
        {
            auto [header, data] = mStorage.readFrame(frameNumber);

            std::string jsonStr = header.toJson().dump();
            uint32_t jsonSize = htonl(jsonStr.size());
            send(clientSocketFd, &jsonSize, sizeof(jsonSize), 0);
            send(clientSocketFd, jsonStr.c_str(), jsonStr.size(), 0);

            uint32_t dataSize = htonl(data.size());
            send(clientSocketFd, &dataSize, sizeof(dataSize), 0);
            send(clientSocketFd, data.data(), data.size(), 0);

            std::cout << "Frame " << frameNumber << " sent successfully." << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error sending frame " << frameNumber << ": " << e.what() << std::endl;
        }
    }

    void Server::sendFramesInRange(int clientSocketFd, int startFrame, int endFrame)
    {
        const auto& headers = mStorage.getVideoHeaders();
        if (headers.empty())
        {
            std::cerr << "No frames available in Storage." << std::endl;
            return;
        }

        int minFrame = headers.front().frameNumber;
        int maxFrame = headers.back().frameNumber;

        std::cout << "Available frames: " << minFrame << " to " << maxFrame << std::endl;

        startFrame = std::max(startFrame, minFrame);
        endFrame = std::min(endFrame, maxFrame);

        for (int frame = startFrame; frame <= endFrame; ++frame)
        {
            sendFrame(clientSocketFd, frame);
        }
    }

    std::pair<int, int> Server::getAvailableFrameRange() const
    {
        const auto& headers = mStorage.getVideoHeaders();
        if (headers.empty())
        {
            return {0, 0};
        }
        return {headers.front().frameNumber, headers.back().frameNumber};
    }
}

