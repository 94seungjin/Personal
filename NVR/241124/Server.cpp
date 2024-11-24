#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <thread> // sleep_for를 사용하기 위해 추가
#include <chrono> // std::chrono 사용을 위해 추가

Server::Server(int port, video::Storage& storage)
    : mStorage(storage), mFps(30)
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket < 0)
    {
        throw std::runtime_error("Failed to create server socket.");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(mServerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("Failed to bind server socket.");
    }

    if (listen(mServerSocket, 5) < 0)
    {
        throw std::runtime_error("Failed to listen on server socket.");
    }

    std::cout << "Server is running on port " << port << std::endl;
}

Server::~Server()
{
    close(mServerSocket);
}

void Server::Run()
{
    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(mServerSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0)
        {
            std::cerr << "Failed to accept connection." << std::endl;
            continue;
        }

        std::cout << "Viewer connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;

        handleViewerRequest(clientSocket);

        close(clientSocket);
    }
}

void Server::handleViewerRequest(int clientSocket)
{
    try
    {
        int startFrame = 0;
        if (recv(clientSocket, &startFrame, sizeof(startFrame), 0) <= 0)
        {
            throw std::runtime_error("Failed to receive start frame number.");
        }
        startFrame = ntohl(startFrame);

        std::cout << "Start streaming from frame: " << startFrame << std::endl;

        streamToViewer(clientSocket, startFrame);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error handling viewer request: " << e.what() << std::endl;
    }
}

void Server::streamToViewer(int clientSocket, int startFrame)
{
    int frameNumber = startFrame;

    while (true)
    {
        try
        {
            video::FrameBody frameBody = mStorage.RetrieveFrame(frameNumber);

            uint32_t frameSize = htonl(frameBody.RawData.size());
            if (send(clientSocket, &frameSize, sizeof(frameSize), 0) <= 0 ||
            send(clientSocket, frameBody.RawData.data(), frameBody.RawData.size(), 0) <= 0)
            {
                throw std::runtime_error("Failed to send frame data.");
            }

            std::cout << "Sent frame " << frameNumber << " to viewer." << std::endl;
            ++frameNumber;
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Streaming error: " << e.what() << ". Retrying in 1 second..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 대기 후 재시도
        }
    }
}

int Server::GetFps() const
{
    return mFps;
}

void Server::SetFps(int fps)
{
    mFps = fps;
}

