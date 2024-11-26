#ifndef VIEWER_H
#define VIEWER_H

#include <vector>
#include <iostream>

class Viewer {
public:
    Viewer();

    // 수신한 프레임 데이터를 화면에 출력
    void displayFrame(int frameNumber);

private:
    // 디스플레이를 위한 내부 구현 (확장 가능)
};

#endif // VIEWER_H

