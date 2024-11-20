#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctime>

using namespace std;

Server::Server(int port, const string &videoFile)
    : mServerSocket(-1), mClientSocket(-1), mPort(port), mVideoFile(videoFile), mbIsClientConnected(false), mStorage(100){}

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
        // 메타데이터 생성
        Metadata metadata;
        metadata.frameNumber = frameCount++;
        metadata.timestamp = std::to_string(std::time(nullptr));
        metadata.width = frame.cols;
        metadata.height = frame.rows;

        // 원본 데이터를 Storage에 저장
        FrameData frameData = {metadata, frame.clone()};
        mStorage.pushFrame(frameData);

        // 큐에서 데이터 가져오기
        if (!mStorage.isEmpty())
        {
            FrameData storedFrame = mStorage.popFrame();

            // 메타데이터 전송
            if (send(mClientSocket, &storedFrame.metadata, sizeof(Metadata), 0) <= 0)
            {
                cerr << "Error sending metadata!" << endl;
                return false;
            }

            // 프레임 데이터 전송
            int dataSize = storedFrame.rawData.total() * storedFrame.rawData.elemSize();
            if (send(mClientSocket, &dataSize, sizeof(dataSize), 0) <= 0)
            {
                cerr << "Error sending data size!" << endl;
                return false;
            }
            if (send(mClientSocket, storedFrame.rawData.data, dataSize, 0) <= 0)
            {
                cerr << "Error sending frame data!" << endl;
                return false;
            }

            cout << "Sent frame " << storedFrame.metadata.frameNumber
                 << " (size: " << dataSize << " bytes)" << endl;
        }

        // FPS 조절
        cv::waitKey(30);
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
