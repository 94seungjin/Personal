#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <memory>
#include "../storage/Storage.hpp"
#include "../common/log/Logger.h"

class Server
{
public:
    Server(int port, const std::string& metadataDir, const std::string& rawdataDir);
    void Start();

private:
    int serverPort;                      // 서버 포트
    int serverSocketFd;            // 서버 소켓 파일 디스크립터
    Storage storage;               // Storage 객체
    static logger::Logger logger;  // Logger 객체

    void setupServer();
    void handleClient(int clientSocketFd);
    void sendMetadata(int clientSocketFd, const Metadata& metadata);
    void sendRawData(int clientSocketFd, const RawData& rawData);
};

#endif // SERVER_HPP

