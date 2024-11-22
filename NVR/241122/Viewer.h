#ifndef VIEWER_H
#define VIEWER_H

#include <string>
#include <opencv2/opencv.hpp>

namespace viewer
{
    class Viewer
    {
    private:
        void receiveFrame(int serverSocketFd); // 서버에서 데이터 수신 및 디코딩

    public:
        Viewer() = default;

        // 서버에서 유효한 프레임 범위 요청
        void requestFrameRange(const std::string& serverIP, int serverPort, int& startFrame, int& endFrame);

        // Viewer 시작
        void Start(int startFrame, int endFrame);
    };
}

#endif // VIEWER_H

