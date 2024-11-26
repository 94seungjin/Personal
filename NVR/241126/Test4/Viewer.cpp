#include "Viewer.h"
#include <iostream>

Viewer::Viewer() {}

void Viewer::displayFrame(int frameNumber) {
    // 단순히 프레임 번호 출력
    std::cout << "Viewer: Displaying FrameNumber: " << frameNumber << "\n";
}

