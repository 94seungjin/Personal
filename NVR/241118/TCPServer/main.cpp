#include "Server.h"
#include <iostream>

using namespace std;

int main()
{
    // 서버 설정
    int port = 8080;
    string videoFile = "storage/cctv.mp4";
	string outputRawFile = "storage/output.raw";

    // Server 객체 생성
    Server server(port, videoFile, outputRawFile);

    // 서버 실행
    server.Start();

    return 0;
}

