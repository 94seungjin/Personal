#include <iostream>
#include "Storage.h"

int main() {
    using namespace video;

    // Storage 초기화
    Storage storage("data_storage", 5); // MaxQueueSize = 5
    std::cout << "Storage 초기화 완료.\n";

    // FrameHeader와 FrameBody 저장
    for (uint32_t i = 1; i <= 5; ++i) {
        FrameHeader frameHeader = {1234567890 + i, i, 640, 480, 1};
        FrameBody frameBody = {{0x01, 0x02, 0x03, 0x04}};

        storage.SaveFrameHeader(frameHeader);
        std::cout << "FrameHeader " << i << " 저장 성공\n";

        storage.SaveFrameBody(frameBody);
        std::cout << "FrameBody " << i << " 저장 성공\n";
    }

    // VideoHeader 저장
    VideoHeader videoHeader = {1234567891, 1234567895, 1, 5, 640, 480};
    storage.SaveVideoHeader(videoHeader);
    std::cout << "VideoHeader 저장 성공\n";

    // 6번째 데이터 추가 (삭제 조건 발생)
    FrameHeader newFrameHeader = {1234567896, 6, 640, 480, 1};
    FrameBody newFrameBody = {{0x05, 0x06, 0x07, 0x08}};

    storage.SaveFrameHeader(newFrameHeader);
    std::cout << "FrameHeader 6 저장 성공\n";

    storage.SaveFrameBody(newFrameBody);
    std::cout << "FrameBody 6 저장 성공\n";

    // 상태 확인
    std::cout << "Storage 상태:\n" << storage.GetStatus() << "\n";

    // VideoHeader 삭제 조건 테스트
    std::cout << "VideoHeader 삭제 조건 테스트 시작\n";

    // 남은 FrameHeader와 FrameBody 삭제
    for (uint32_t i = 2; i <= 6; ++i) {
        std::string frameHeaderPath = "data_storage/frame_headers/frame_header_" + std::to_string(i) + ".json";
        std::string frameBodyPath = "data_storage/frame_bodies/frame_body_" + std::to_string(i) + ".bin";

        if (std::filesystem::remove(frameHeaderPath)) {
            std::cout << "FrameHeader " << i << " 삭제 완료\n";
        }
        if (std::filesystem::remove(frameBodyPath)) {
            std::cout << "FrameBody " << i << " 삭제 완료\n";
        }
    }

    // 상태 확인
    std::cout << "Storage 상태:\n" << storage.GetStatus() << "\n";

    return 0;
}
