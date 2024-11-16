#include "Viewer.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

Viewer::Viewer(const string &serverIp, int serverPort)
    : mServerIp(serverIp), mServerPort(serverPort), mSocket(-1), bIsConnected(false), bStopRequested(false) {}

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
    bIsConnected = true;
    return true;
}

void Viewer::Start()
{
    if (!ConnectToServer())
    {
        cerr << "Unable to connect to server." << endl;
        return;
    }

    vector<uchar> buffer;
    cv::Mat frame;
    int size;

    while (!bStopRequested) // 종료 조건 추가
    {
        // Receive size of the incoming frame
        int valread = recv(mSocket, &size, sizeof(size), 0);
        if (valread <= 0)
        {
            cerr << "Connection closed or error receiving size!" << endl;
            break;
        }

        // Receive frame data
        buffer.resize(size);
        int bytesRead = 0;
        while (bytesRead < size)
        {
            valread = recv(mSocket, buffer.data() + bytesRead, size - bytesRead, 0);
            if (valread <= 0)
            {
                cerr << "Error receiving frame data!" << endl;
                return;
            }
            bytesRead += valread;
        }

        // Decode and display frame
        frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
        if (frame.empty())
        {
            cerr << "Failed to decode frame!" << endl;
            break;
        }

        DisplayFrame(frame);

        // 사용자 입력에 따라 종료를 방지
        if (cv::waitKey(30) == 'q') // 'q' 키로 종료
            Stop();
    }

    Cleanup();
}

void Viewer::Stop()
{
    bStopRequested = true; // 종료 플래그 설정
    cout << "Stop requested." << endl;
}

void Viewer::DisplayFrame(const cv::Mat &frame)
{
    cv::imshow("Client View", frame);
}

void Viewer::Cleanup()
{
    if (bIsConnected)
    {
        close(mSocket);
        bIsConnected = false;
        cout << "Socket closed." << endl;
    }
    cv::destroyAllWindows();
    cout << "OpenCV windows destroyed." << endl;
}

