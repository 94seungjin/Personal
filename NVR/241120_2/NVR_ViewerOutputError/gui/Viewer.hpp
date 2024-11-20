#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <string>
#include <vector>
#include "../common/log/Logger.h"

class Viewer
{
public:
    Viewer(const std::string& serverIP, int serverPort);
    void Start();

private:
    std::string serverIP;
    int serverPort;
    int clientSocketFd;
    static logger::Logger logger;

    void connectToServer();
    void receiveAndProcessData();
    void processMetadata(const std::string& metadata);
    void processRawData(const std::vector<uint8_t>& rawData);
};

#endif // VIEWER_HPP

