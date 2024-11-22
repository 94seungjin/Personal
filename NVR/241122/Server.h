#ifndef SERVER_H
#define SERVER_H

#include "Storage.h"
#include <string>
#include <netinet/in.h>

namespace viewer
{
    class Server
    {
    private:
        video::Storage& mStorage;       // Storage 참조
        int mPort;                      // 서버 포트
        int mServerSocketFd;            // 서버 소켓 fd

        void setupServer();             // 서버 소켓 설정
        void sendFrame(int clientSocketFd, int frameNumber); // 특정 프레임 전송
        void sendFramesInRange(int clientSocketFd, int startFrame, int endFrame); // 특정 범위 프레임 전송

    public:
        Server(video::Storage& storage, int port);
        void Start();                   // 서버 실행
        std::pair<int, int> getAvailableFrameRange() const; // 유효한 프레임 범위 반환
    };
}

#endif // SERVER_H

