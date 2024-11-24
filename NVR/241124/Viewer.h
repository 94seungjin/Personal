#ifndef VIEWER_H
#define VIEWER_H

#include <string>
#include <vector>

class Viewer
{
private:
    std::string mServerAddress;     // 서버 주소 (IP)
    int mPort;                      // 서버 포트 번호

    /**
     * 수신된 데이터를 처리
     * @param frameData 수신된 프레임 데이터
     */
    void processFrameData(const std::vector<uint8_t>& frameData);

public:
    /**
     * Viewer 생성자
     * @param serverAddress 서버 주소 (IP)
     * @param port 서버 포트 번호
     */
    Viewer(const std::string& serverAddress, int port);

    /**
     * Viewer 실행 함수
     * @param startFrame 시작 프레임 번호
     */
    void Run(int startFrame);
};

#endif // VIEWER_H

