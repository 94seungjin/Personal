#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

Server::Server(int port, const string &videoFile)
    : mServerSocket(-1), mClientSocket(-1), mPort(port), mVideoFile(videoFile), bIsClientConnected(false) {}

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
    bIsClientConnected = true;
    return true;
}

void Server::Start()
{
    if (!InitializeServer())
    {
        cerr << "Failed to initialize server." << endl;
        return;
    }

    if (!StreamVideo())
    {
        cerr << "Error while streaming video." << endl;
    }

    Cleanup();
}

bool Server::StreamVideo()
{
    cv::VideoCapture cap(mVideoFile);
    if (!cap.isOpened())
    {
        cerr << "Unable to open video file: " << mVideoFile << endl;
        return false;
    }

    cv::Mat frame;
    vector<uchar> buffer;

    while (cap.read(frame))
    {
        // 프레임을 JPEG로 인코딩
        cv::imencode(".jpg", frame, buffer);
        int size = buffer.size();

        // 크기 전송
        if (send(mClientSocket, &size, sizeof(size), 0) <= 0)
        {
            cerr << "Error sending frame size!" << endl;
            return false;
        }

        // 데이터 전송
        if (send(mClientSocket, buffer.data(), size, 0) <= 0)
        {
            cerr << "Error sending frame data!" << endl;
            return false;
        }

        // 대기
        cv::waitKey(30); // FPS 조절
    }

    cout << "Video streaming completed!" << endl;
    return true;
}

void Server::Cleanup()
{
    if (mClientSocket != -1)
    {
        close(mClientSocket);
        mClientSocket = -1;
        bIsClientConnected = false;
        cout << "Client connection closed." << endl;
    }

    if (mServerSocket != -1)
    {
        close(mServerSocket);
        mServerSocket = -1;
        cout << "Server socket closed." << endl;
    }
}

