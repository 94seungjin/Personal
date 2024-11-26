#include "Server.h"
#include <iostream>

Server::Server(video::Storage& storage) : mStorage(storage) {}

void Server::fetchAndSendToViewer(int frameNumber) {
    // FrameHeader 검색
    video::FrameHeader header;
    if (mStorage.SearchFrameHeader(1234567890 + frameNumber, header)) {
        std::cout << "Server: FrameHeader 검색 성공 - FrameNumber: " << frameNumber << "\n";
    } else {
        std::cerr << "Server: FrameHeader 검색 실패 - FrameNumber: " << frameNumber << "\n";
        return;
    }

    // FrameBody 검색
    video::FrameBody body;
    if (mStorage.SearchFrameBody(static_cast<uint32_t>(frameNumber), body)) {
        std::cout << "Server: FrameBody 검색 성공 - FrameNumber: " << frameNumber << "\n";
    } else {
        std::cerr << "Server: FrameBody 검색 실패 - FrameNumber: " << frameNumber << "\n";
        return;
    }

    // 데이터 송신 (단순 출력으로 시뮬레이션)
    std::cout << "Server: FrameHeader 송신 성공\n";
    std::cout << "Server: FrameBody 송신 성공\n";
}

