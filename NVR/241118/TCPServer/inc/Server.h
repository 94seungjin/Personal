#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Server
{
public:
    Server(int port, const std::string &videoFile, const std::string &outputRawFile);
    ~Server();

    void Start();

private:
    int mServerSocket;
    int mClientSocket;
    int mPort;
    std::string mVideoFile;
    std::string mOutputRawFile;
    bool mbIsClientConnected;

    bool InitializeServer();
    void streamVideo();
    void cleanup();
};

#endif // SERVER_H

