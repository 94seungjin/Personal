#ifndef VIEWER_H
#define VIEWER_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class Viewer
{
public:
    Viewer(const std::string &serverIp, int serverPort);
    ~Viewer();

    void Start();

private:
    int mSocket;
    std::string mServerIp;
    int mServerPort;
    int mWidth;
    int mHeight;

    bool ConnectToServer();
    void receiveAndProcessVideo();
    void cleanup();
};

#endif // VIEWER_H

