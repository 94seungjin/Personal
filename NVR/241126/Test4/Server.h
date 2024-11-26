#ifndef SERVER_H
#define SERVER_H

#include "Storage.h"

class Server {
public:
    explicit Server(video::Storage& storage);

    // Storage에서 데이터를 가져와 Viewer로 전송
    void fetchAndSendToViewer(int frameNumber);

private:
    video::Storage& mStorage; // Storage와의 연결
};

#endif // SERVER_H

