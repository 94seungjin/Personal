#include "Client.h"
#include <iostream>

Client::Client(video::Storage& storage) : mStorage(storage) {}

void Client::receiveAndStoreFrame(int frameNumber) {
    video::FrameHeader header{1234567890ULL + frameNumber, static_cast<uint32_t>(frameNumber), 640, 480, 1};
    if (mStorage.SaveFrameHeader(header)) {
        std::cout << "Client: FrameHeader 저장 성공 - FrameNumber: " << frameNumber << "\n";
    }

    video::FrameBody body{{0x01, 0x02, 0x03, 0x04}};
    if (mStorage.SaveFrameBody(body)) {
        std::cout << "Client: FrameBody 저장 성공 - FrameNumber: " << frameNumber << "\n";
    }
}

