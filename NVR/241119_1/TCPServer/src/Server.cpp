#include "Server.hpp"
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

Server::Server(int port, const string &videoFile)
    : mServerSocket(-1), mClientSocket(-1), mPort(port), mVideoFile(videoFile), mbIsClientConnected(false) {}

Server::~Server()
{
    Cleanup();
}

bool Server::InitializeServer()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket == 0)
    {
        cerr << "Socket creation failed!" << endl;
        return false;
    }

    struct sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(mPort);

    if (bind(mServerSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cerr << "Bind failed!" << endl;
        return false;
    }

    if (listen(mServerSocket, 3) < 0)
    {
        cerr << "Listen failed!" << endl;
        return false;
    }

    cout << "Waiting for client connection..." << endl;
    int addrlen = sizeof(address);
    mClientSocket = accept(mServerSocket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (mClientSocket < 0)
    {
        cerr << "Accept failed!" << endl;
        return false;
    }

    cout << "Client connected!" << endl;
    mbIsClientConnected = true;
    return true;
}

void Server::Start()
{
    if (!InitializeServer())
    {
        cerr << "Failed to initialize server." << endl;
        return;
    }

    if (!ExtractRawVideoData())
    {
        cerr << "Error while extracting raw video data." << endl;
    }

    Cleanup();
}

bool Server::ExtractRawVideoData()
{
    cv::VideoCapture cap(mVideoFile);
    if (!cap.isOpened())
    {
        cerr << "Unable to open video file: " << mVideoFile << endl;
        return false;
    }

    cv::Mat frame;
    int frameCount = 0;

    while (cap.read(frame))
    {
        // RGB로 변환
        cv::Mat rgbFrame;
        cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);

        // 프레임 데이터 크기 계산
        int rows = rgbFrame.rows;
        int cols = rgbFrame.cols;
        int channels = rgbFrame.channels();
        int dataSize = rows * cols * channels;

        // 클라이언트로 데이터 전송
        // 1. 프레임의 행(row) 전송
        if (send(mClientSocket, &rows, sizeof(rows), 0) <= 0)
        {
            cerr << "Error sending frame rows!" << endl;
            return false;
        }

        // 2. 프레임의 열(column) 전송
        if (send(mClientSocket, &cols, sizeof(cols), 0) <= 0)
        {
            cerr << "Error sending frame cols!" << endl;
            return false;
        }

        // 3. 프레임의 채널 수 전송
        if (send(mClientSocket, &channels, sizeof(channels), 0) <= 0)
        {
            cerr << "Error sending frame channels!" << endl;
            return false;
        }

        // 4. 픽셀 데이터 전송
        if (send(mClientSocket, rgbFrame.data, dataSize, 0) <= 0)
        {
            cerr << "Error sending frame data!" << endl;
            return false;
        }

        cout << "Sent frame " << ++frameCount << " (size: " << dataSize << " bytes)" << endl;

        // FPS 조절
        cv::waitKey(30); // 약 33 FPS
    }

    cout << "Raw video data streaming completed!" << endl;
    return true;
}


void Server::Cleanup()
{
    if (mClientSocket != -1)
    {
        close(mClientSocket);
        mClientSocket = -1;
        mbIsClientConnected = false;
        cout << "Client connection closed." << endl;
    }

    if (mServerSocket != -1)
    {
        close(mServerSocket);
        mServerSocket = -1;
        cout << "Server socket closed." << endl;
    }
}
