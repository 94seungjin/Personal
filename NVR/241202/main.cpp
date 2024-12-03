#include <iostream>
#include "VideoHeader.h"
#include "FrameBody.h"
#include "FrameHeader.h"
#include "FrameQueue.h"
#include "Storage.h"

void TestVideoStorageWorkflow()
{
    std::cout << "=== Video Storage Workflow Test ===" << std::endl;

    // 1. 비디오 헤더 초기화
    std::cout << "1. Initializing Video Header..." << std::endl;
    VideoHeader videoHeader;
    videoHeader.setVideoID(1234);
    videoHeader.setResolution(1920, 1080);
    videoHeader.setFrameRate(15);
    videoHeader.setVideoName("20241201_120000.000");

    std::cout << "Video Header Initialized: " << std::endl;
    std::cout << "Video ID: " << videoHeader.getVideoID() << std::endl;
    std::cout << "Resolution: " << videoHeader.getResolutionWidth() << "x" << videoHeader.getResolutionHeight() << std::endl;
    std::cout << "Frame Rate: " << videoHeader.getFrameRate() << " fps" << std::endl;
    std::cout << "Video Name: " << videoHeader.getVideoName() << std::endl;

    // 2. Storage 객체 생성 및 비디오 초기화
    std::cout << "\n2. Creating Storage and Initializing Video Storage..." << std::endl;
    Storage storage("video_storage.bin");
    storage.initializeVideoStorage(videoHeader);
    std::cout << "Video Storage Initialized Successfully!" << std::endl;

    // 3. 프레임 저장
    std::cout << "\n3. Saving Frames to Storage..." << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        FrameHeader frameHeader;
        frameHeader.setTimestamp(i * 1000); // timestamp 설정
        frameHeader.setFrameID(i);         // frame ID 설정
        frameHeader.setSize(1024);         // frame 크기 설정

        FrameBody frameBody;
        std::string frameData = "Frame Data " + std::to_string(i); // 프레임 데이터
        frameBody.setData(frameData.c_str(), frameData.size());

        storage.saveFrame(frameHeader, frameBody);
        std::cout << "Saved Frame " << i << " to Storage." << std::endl;
    }

    // 4. 저장된 프레임 읽기
    std::cout << "\n4. Reading Frames from Storage..." << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        FrameHeader frameHeader;
        FrameBody frameBody;
        if (storage.readFrame(i, frameHeader, frameBody))
        {
            std::cout << "Read Frame " << i << " - Timestamp: " << frameHeader.getTimestamp()
                      << ", FrameID: " << frameHeader.getFrameID()
                      << ", Data: " << frameBody.getData() << std::endl;
        }
        else
        {
            std::cout << "Failed to Read Frame " << i << std::endl;
        }
    }

    // 5. 비디오 저장 종료
    std::cout << "\n5. Finalizing Video Storage..." << std::endl;
    storage.finalizeVideoStorage();
    std::cout << "Video Storage Finalized Successfully!" << std::endl;

    std::cout << "=== Video Storage Workflow Test Completed ===" << std::endl;
}

int main()
{
    std::cout << "=== Start Testing ===" << std::endl;

    TestVideoStorageWorkflow();

    std::cout << "=== End Testing ===" << std::endl;
    return 0;
}
