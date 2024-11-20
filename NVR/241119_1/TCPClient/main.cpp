#include "Viewer.hpp"
#include <thread>
#include <chrono>

using namespace std;

int main()
{
    Viewer viewer("127.0.0.1", 8080);

    // Viewer 실행
    thread viewerThread([&viewer]() { viewer.Start(); });

    // 프로그램이 바로 종료되지 않도록 대기
    cout << "Press 'q' in the viewer window to stop." << endl;

    // Viewer 종료 대기
    viewerThread.join();

    return 0;
}

