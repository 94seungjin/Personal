#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class Viewer
{
public:
    Viewer(const std::string &serverIp, int serverPort);
    ~Viewer();
    void Start();
    void Stop();

private:
    int mSocket;
    std::string mServerIp;
    int mServerPort;
    bool mbIsConnected;
    bool mbStopRequested;

    bool ConnectToServer();
    void DisplayFrame(const cv::Mat &frame);
    void Cleanup();
};

#endif // VIEWER_HPP

