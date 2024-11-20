#include "Viewer.hpp"
#include "../../TCPServer/inc/Storage.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

Viewer::Viewer(const string &serverIp, int serverPort)
    : mServerIp(serverIp), mServerPort(serverPort), mSocket(-1), mbIsConnected(false), mbStopRequested(false) {}

Viewer::~Viewer()
{
    Cleanup();
}

bool Viewer::ConnectToServer()
{
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0)
    {
        cerr << "Socket creation error!" << endl;
        return false;
    }

    struct sockaddr_in serv_addr = {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(mServerPort);

    if (inet_pton(AF_INET, mServerIp.c_str(), &serv_addr.sin_addr) <= 0)
    {
        cerr << "Invalid server address!" << endl;
        return false;
    }

    if (connect(mSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cerr << "Connection to server failed!" << endl;
        return false;
    }

    cout << "Connected to server!" << endl;
    mbIsConnected = true;
    return true;
}

void Viewer::Start()
{
    if (!ConnectToServer())
    {
        cerr << "Unable to connect to server." << endl;
        return;
    }

    int previousFrameNumber = -1; // 이전 frameNumber 저장

    while (!mbStopRequested)
    {
        Metadata metadata;
        int dataSize;

        // 1. Metadata 수신
        int recvBytes = recv(mSocket, &metadata, sizeof(Metadata), 0);
        if (recvBytes != sizeof(Metadata))
        {
            cerr << "Metadata recv error: received " << recvBytes 
                 << " bytes, expected " << sizeof(Metadata) << endl;
            break;
        }

        cout << "Metadata received: frameNumber=" << metadata.frameNumber
             << ", height=" << metadata.height
             << ", width=" << metadata.width << endl;

        // 동일한 frameNumber 반복 시 처리 중단
        if (metadata.frameNumber == previousFrameNumber)
        {
            cerr << "Duplicate frameNumber received: " << metadata.frameNumber << endl;
            continue;
        }
        previousFrameNumber = metadata.frameNumber;

        // 2. 프레임 데이터 크기 수신
        if (recv(mSocket, &dataSize, sizeof(dataSize), 0) <= 0)
        {
            cerr << "Connection closed or error receiving data size!" << endl;
            break;
        }
        cout << "Data size received: " << dataSize << " bytes" << endl;

        // 데이터 크기 검증
        if (dataSize <= 0)
        {
            cerr << "Invalid frame data size received: " << dataSize << endl;
            break;
        }

        // 3. 프레임 데이터 수신
        vector<uchar> buffer(dataSize);
        int bytesRead = 0;

        while (bytesRead < dataSize)
        {
            int valread = recv(mSocket, buffer.data() + bytesRead, dataSize - bytesRead, 0);
            if (valread <= 0)
            {
                cerr << "Error receiving pixel data!" << endl;
                break;
            }
            bytesRead += valread;
        }
        cout << "Bytes received: " << bytesRead << "/" << dataSize << " bytes" << endl;

        if (bytesRead != dataSize)
        {
            cerr << "Incomplete frame data received! Expected=" << dataSize
                 << ", Received=" << bytesRead << endl;
            break;
        }

        // 4. 프레임 디스플레이
        cv::Mat frame(metadata.height, metadata.width, CV_8UC3, buffer.data());
        cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
        cv::imshow("Client View", frame);

        if (cv::waitKey(30) == 'q') // 'q' 키로 종료
            Stop();
    }

    Cleanup();
}

void Viewer::Stop()
{
    mbStopRequested = true; // 종료 플래그 설정
    cout << "Stop requested." << endl;
}

void Viewer::DisplayFrame(const cv::Mat &frame)
{
    // OpenCV 창 표시
    if (frame.empty())
    {
        cerr << "Frame is empty. Skipping display!" << endl;
        return;
    }

    cv::imshow("Client View", frame);

    // OpenCV GUI 함수는 반드시 메인 스레드에서 호출
    cv::waitKey(30);
}

void Viewer::Cleanup()
{
    if (mbIsConnected)
    {
        close(mSocket);
        mbIsConnected = false;
        cout << "Socket closed." << endl;
    }
    cv::destroyAllWindows();
    cout << "OpenCV windows destroyed." << endl;
}

