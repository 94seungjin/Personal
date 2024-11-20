#ifndef SERVER_HPP
#define SERVER_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class Server
{
public:
    Server(int port, const std::string &videoFile);
    ~Server();

    void Start();

private:
    int mServerSocket;
    int mClientSocket;
    int mPort;
    std::string mVideoFile;

    bool mbIsClientConnected;

    bool InitializeServer();
    void Cleanup();
    bool StreamVideo();
};

#endif // SERVER_HPP

