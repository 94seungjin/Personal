#include "Server.h"

Server::Server(int port, const std::string &videoFile, const std::string &outputRawFile)
    : mPort(port), mVideoFile(videoFile), mOutputRawFile(outputRawFile),
      mServerSocket(-1), mClientSocket(-1), mbIsClientConnected(false) {}

Server::~Server()
{
    cleanup();
}

bool Server::InitializeServer()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket < 0)
    {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(mPort);

    if (bind(mServerSocket, (sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed!" << std::endl;
        return false;
    }

    if (listen(mServerSocket, 3) < 0)
    {
        std::cerr << "Listen failed!" << std::endl;
        return false;
    }

    std::cout << "Waiting for client connection..." << std::endl;
    int addrlen = sizeof(address);
    mClientSocket = accept(mServerSocket, (sockaddr *)&address, (socklen_t *)&addrlen);
    if (mClientSocket < 0)
    {
        std::cerr << "Client accept failed!" << std::endl;
        return false;
    }

    std::cout << "Client connected!" << std::endl;
    mbIsClientConnected = true;
    return true;
}

void Server::streamVideo()
{
    // 영상 메타데이터 전송
    int width = 640;
    int height = 480;
    if (send(mClientSocket, &width, sizeof(width), 0) <= 0 ||
        send(mClientSocket, &height, sizeof(height), 0) <= 0)
    {
        std::cerr << "Error sending video metadata!" << std::endl;
        return;
    }

    std::ifstream videoFile(mVideoFile, std::ios::binary);
    if (!videoFile.is_open())
    {
        std::cerr << "Failed to open video file: " << mVideoFile << std::endl;
        return;
    }

    std::ofstream rawFile(mOutputRawFile, std::ios::binary);
    if (!rawFile.is_open())
    {
        std::cerr << "Failed to open raw output file: " << mOutputRawFile << std::endl;
        return;
    }

    const size_t bufferSize = 4096;
    char buffer[bufferSize];

    while (videoFile.read(buffer, bufferSize) || videoFile.gcount() > 0)
    {
        size_t bytesToSend = videoFile.gcount();

        // 클라이언트로 전송
        if (send(mClientSocket, &bytesToSend, sizeof(bytesToSend), 0) <= 0 ||
            send(mClientSocket, buffer, bytesToSend, 0) <= 0)
        {
            std::cerr << "Error sending frame data!" << std::endl;
            break;
        }

        // 로컬에 Raw Data 저장
        rawFile.write(buffer, bytesToSend);
    }

    videoFile.close();
    rawFile.close();
    std::cout << "Streaming and saving completed." << std::endl;
}

void Server::Start()
{
    if (!InitializeServer())
    {
        std::cerr << "Failed to initialize server." << std::endl;
        return;
    }

    streamVideo();
    cleanup();
}

void Server::cleanup()
{
    if (mClientSocket != -1)
    {
        close(mClientSocket);
        mClientSocket = -1;
        std::cout << "Client connection closed." << std::endl;
    }

    if (mServerSocket != -1)
    {
        close(mServerSocket);
        mServerSocket = -1;
        std::cout << "Server socket closed." << std::endl;
    }
}

