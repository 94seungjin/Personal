#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include "Storage.h"

class Server
{
private:
    int mServerSocket;              // 서버 소켓 파일 디스크립터
    video::Storage& mStorage;       // Storage 객체 참조
    int mFps = 30;                  // 초당 프레임 수

    /**
     * Viewer 요청을 처리하는 함수
     * @param clientSocket 클라이언트 소켓 파일 디스크립터
     */
    void handleViewerRequest(int clientSocket);

    /**
     * Viewer로 실시간 스트리밍 데이터를 전송하는 함수
     * @param clientSocket 클라이언트 소켓 파일 디스크립터
     * @param startFrame 시작 프레임 번호
     */
    void streamToViewer(int clientSocket, int startFrame);

public:
    /**
     * Server 생성자
     * @param port 서버가 실행될 포트 번호
     * @param storage Storage 객체 참조
     */
    Server(int port, video::Storage& storage);

    /**
     * Server 소멸자
     */
    ~Server();

    /**
     * Server 실행 함수
     * - 클라이언트 연결을 대기하고 요청을 처리
     */
    void Run();

    /**
     * FPS Getter
     * @return 초당 프레임 수
     */
    int GetFps() const;

    /**
     * FPS Setter
     * @param fps 설정할 초당 프레임 수
     */
    void SetFps(int fps);
};

#endif // SERVER_H

