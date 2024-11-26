#include <opencv2/opencv.hpp>
#include <iostream>
#include "Storage.h"

using namespace video;

int main()
{
    // 카메라 캡처 설정
    cv::VideoCapture cap(0); // 0번 카메라 (파이 카메라)
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 30);

    // Storage 객체 생성
    std::string storageDir = "./storage";
    size_t maxQueueSize = 100;
    Storage storage(storageDir, maxQueueSize);

    int frameCounter = 0;
    while (frameCounter < 100)
    { // 제한된 프레임 수로 테스트
        cv::Mat frame;
        cap >> frame; // 프레임 캡처

        if (frame.empty())
        {
            std::cerr << "Error: Captured an empty frame." << std::endl;
            continue;
        }

        // OpenCV에서 Mat 데이터를 바이너리로 변환
        std::vector<uint8_t> buffer;
        cv::imencode(".jpg", frame, buffer); // JPG 포맷으로 인코딩

        // 프레임 데이터 생성
        FrameHeader header =
        {
            static_cast<uint64_t>(time(nullptr)), // 현재 타임스탬프
            static_cast<uint32_t>(frameCounter),  // 프레임 번호
            static_cast<uint16_t>(frame.cols),    // 가로 해상도
            static_cast<uint16_t>(frame.rows),    // 세로 해상도
            "JPEG"                                // 압축 방식
        };

        FrameBody body = { buffer };

        // Storage에 프레임 추가
        storage.AddFrame(header, body);

        // 프레임 상태 출력
        std::cout << "Added Frame: " << header.frameNumber << std::endl;
        std::cout << "Queue Size: " << storage.GetQueueSize() << std::endl;

        // 특정 프레임 데이터 조회 및 출력
        size_t frameStartPointer;
        if (frameCounter == 5)
        { // 특정 조건에서 탐색
            if (storage.SeekToFrame(5, frameStartPointer))
            {
                std::cout << "Frame 5 found at start pointer: " << frameStartPointer << std::endl;
            }
            else
            {
                std::cerr << "Frame 5 not found!" << std::endl;
            }
        }

        frameCounter++;
    }

    cap.release();

    // 저장된 데이터의 무결성 검증
    if (storage.ValidateData())
    {
        std::cout << "All data is consistent." << std::endl;
    }
    else
    {
        std::cerr << "Data inconsistency detected!" << std::endl;
    }
    
    // frameEndPointer 점검
    std::cout << "=== FrameEndPointer Validation ===" << std::endl;
    storage.ValidateFrameEndPointers();
    std::cout << "=== Validation Complete ===" << std::endl;

    // 디스크에 저장된 데이터를 확인
    std::cout << "Saved data in storage directory: " << storageDir << std::endl;

    return 0;
}
