#include "Viewer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>

Viewer::Viewer(const std::string& serverAddress, int port)
    : mServerAddress(serverAddress), mPort(port) {}

void Viewer::processFrameData(const std::vector<uint8_t>& frameData)
{
    std::ofstream outFile("received_frame.jpg", std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(frameData.data()), frameData.size());
    outFile.close();

    std::cout << "Received and saved a frame as received_frame.jpg" << std::endl;
}

void Viewer::Run(int startFrame)
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        throw std::runtime_error("Failed to create client socket.");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(mPort);
    inet_pton(AF_INET, mServerAddress.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("Failed to connect to server.");
    }

    int networkFrame = htonl(startFrame);
    if (send(clientSocket, &networkFrame, sizeof(networkFrame), 0) <= 0)
    {
        throw std::runtime_error("Failed to send start frame number.");
    }

    while (true)
    {
        try
        {
            uint32_t frameSize;
            if (recv(clientSocket, &frameSize, sizeof(frameSize), 0) <= 0)
            {
                throw std::runtime_error("Failed to receive frame size.");
            }
            frameSize = ntohl(frameSize);

            std::vector<uint8_t> frameData(frameSize);
            if (recv(clientSocket, frameData.data(), frameSize, 0) <= 0)
            {
                throw std::runtime_error("Failed to receive frame data.");
            }

            processFrameData(frameData);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Viewer error: " << e.what() << std::endl;
            break;
        }
    }

    close(clientSocket);
}

