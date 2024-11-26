#ifndef CLIENT_H
#define CLIENT_H

#include "Storage.h"

class Client {
public:
    explicit Client(video::Storage& storage);
    
    // Edge에서 데이터를 수신하고 Storage에 저장
    void receiveAndStoreFrame(int frameNumber);

private:
    video::Storage& mStorage; // Storage와의 연결
};

#endif // CLIENT_H

