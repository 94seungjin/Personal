#include "Storage.h"
#include <iostream>

int main()
{
    video::Storage storage("data_storage", 5);

    // 1. FrameHeader와 FrameBody 저장
    for (uint32_t i = 1; i <= 5; ++i)
    {
        video::FrameHeader frameHeader = {1234567890 + i - 1, i, 640, 480, 1};
        video::FrameBody frameBody = {{static_cast<uint8_t>(i), static_cast<uint8_t>(i + 1)}};
        
        if (storage.SaveFrameHeader(frameHeader))
            std::cout << "FrameHeader " << i << " 저장 성공\n";
        if (storage.SaveFrameBody(frameBody))
            std::cout << "FrameBody " << i << " 저장 성공\n";
    }

    // 2. VideoHeader 저장
    video::VideoHeader videoHeader = {1234567890, 1234567894, 1, 5, 640, 480};
    if (storage.SaveVideoHeader(videoHeader))
        std::cout << "VideoHeader 저장 성공\n";

    // 3. 저장된 데이터 검색
    video::VideoHeader searchedVideoHeader;
    if (storage.SearchVideoHeader(1234567892, searchedVideoHeader))
        std::cout << "VideoHeader 검색 성공: " << searchedVideoHeader.ToJson() << "\n";

    // 4. 오래된 데이터 삭제 테스트
    for (uint32_t i = 6; i <= 10; ++i)
    {
        video::FrameHeader frameHeader = {1234567900 + i - 6, i, 640, 480, 1};
        video::FrameBody frameBody = {{static_cast<uint8_t>(i), static_cast<uint8_t>(i + 1)}};
        
        if (storage.SaveFrameHeader(frameHeader))
            std::cout << "FrameHeader " << i << " 저장 성공\n";
        if (storage.SaveFrameBody(frameBody))
            std::cout << "FrameBody " << i << " 저장 성공\n";
    }

    // 상태 출력
    std::cout << "Storage 상태: \n" << storage.GetStatus() << "\n";

    return 0;
}
