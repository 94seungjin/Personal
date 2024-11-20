#include "Viewer.hpp"
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

    while (!mbStopRequested)
    {
        int rows, cols, channels;

        // 1. 행(row) 데이터 수신
        if (recv(mSocket, &rows, sizeof(rows), 0) <= 0)
        {
            cerr << "Connection closed or error receiving rows!" << endl;
            break;
        }

        // 2. 열(column) 데이터 수신
        if (recv(mSocket, &cols, sizeof(cols), 0) <= 0)
        {
            cerr << "Connection closed or error receiving cols!" << endl;
            break;
        }

        // 3. 채널 수 데이터 수신
        if (recv(mSocket, &channels, sizeof(channels), 0) <= 0)
        {
            cerr << "Connection closed or error receiving channels!" << endl;
            break;
        }

        // 4. 픽셀 데이터 수신
        int dataSize = rows * cols * channels;
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

        if (bytesRead != dataSize)
        {
            cerr << "Incomplete frame data received!" << endl;
            break;
        }

		// 5. 픽셀 데이터를 cv::Mat으로 변환
		cv::Mat frame(rows, cols, CV_8UC3, buffer.data());

		// 6. 색상 순서 변환 (RGB → BGR)
		cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

		// 7. 프레임 화면에 표시
		DisplayFrame(frame);

        // 사용자 입력으로 종료
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
    cv::imshow("Client View", frame);
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

