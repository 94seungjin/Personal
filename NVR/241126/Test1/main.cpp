#include "Storage.h"
#include <iostream>

int main() {
    // 1. Storage 초기화
    video::Storage storage("data_storage", 10);

    // 2. FrameHeader 생성
    video::FrameHeader header = { 1234567890, 1, 640, 480, 1 };
    if (storage.SaveFrameHeader(header)) {
        std::cout << "FrameHeader 저장 성공\n";
    }

    // 3. FrameBody 생성
    video::FrameBody body;
    body.RawData = {0x01, 0x02, 0x03, 0x04}; // 간단한 이진 데이터
    if (storage.SaveFrameBody(body)) {
        std::cout << "FrameBody 저장 성공\n";
    }

    // 4. FrameHeader 검색
    video::FrameHeader searchedHeader;
    if (storage.SearchFrameHeader(1234567890, searchedHeader)) {
        std::cout << "FrameHeader 검색 성공: " << searchedHeader.ToJson() << "\n";
    }

    // 5. FrameBody 검색
    video::FrameBody searchedBody;
    if (storage.SearchFrameBody(1, searchedBody)) {
        std::cout << "FrameBody 검색 성공. 크기: " << searchedBody.RawData.size() << "\n";
    }

    // 6. FrameHeader 송신
    if (storage.SendFrameHeader(searchedHeader)) {
        std::cout << "FrameHeader 송신 성공\n";
    }

    // 7. FrameBody 송신
    if (storage.SendFrameBody(searchedBody)) {
        std::cout << "FrameBody 송신 성공\n";
    }

    return 0;
}

