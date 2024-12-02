#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstring> // for strncpy
#include "VideoHeader.h"
#include "FrameHeader.h"
#include "FrameBody.h"
#include "Storage.h"

// 타임스탬프 생성 함수
std::string GenerateTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm;
    localtime_r(&timeT, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    std::string timestamp = oss.str();

    // 정확히 18자로 제한
    if (timestamp.size() > 18)
    {
        timestamp = timestamp.substr(0, 18);
    }

    return timestamp;
}

int main()
{
    try
    {
        // 1. 비디오 헤더 생성 및 초기화
        video::VideoHeader videoHeader;
        videoHeader.totalFrames = 0;
        videoHeader.width = 1920;
        videoHeader.height = 1080;
        videoHeader.frameRate = 15;
        strncpy(videoHeader.creationTime, "20241201_150000.00", sizeof(videoHeader.creationTime) - 1);
        videoHeader.creationTime[sizeof(videoHeader.creationTime) - 1] = '\0';
        videoHeader.startPoint = 0;
        videoHeader.endPoint = 0;

        std::string filePath = "video_test.bin";
        size_t maxFrames = 10;
        video::Storage storage(filePath, maxFrames);

        std::cout << "Initializing video storage..." << std::endl;
        storage.InitializeVideo(videoHeader);

        // 2. 프레임 데이터 생성 및 저장
        for (uint32_t i = 0; i < 15; ++i)
	{
            video::FrameHeader frameHeader;
            frameHeader.frameId = i;
            frameHeader.bodySize = 1024;

            std::string timestamp = GenerateTimestamp();
            strncpy(frameHeader.timestamp, timestamp.c_str(), sizeof(frameHeader.timestamp) - 1);
            frameHeader.timestamp[sizeof(frameHeader.timestamp) - 1] = '\0';

            video::FrameBody frameBody;
            frameBody.data = std::vector<uint8_t>(1024, static_cast<uint8_t>(i)); // 바디 데이터를 i로 채움

            storage.SaveFrame(frameHeader, frameBody);
            std::cout << "Saved frame " << i << " with timestamp " << frameHeader.timestamp << std::endl;
        }

        // 3. 저장된 프레임 읽기 테스트
        video::FrameHeader retrievedHeader;
        video::FrameBody retrievedBody;

        std::cout << "Retrieving frames from storage..." << std::endl;
        while (storage.GetNextFrame(retrievedHeader, retrievedBody)) {
            std::cout << "Frame ID: " << retrievedHeader.frameId
                      << ", Body Size: " << retrievedHeader.bodySize
                      << ", Timestamp: " << retrievedHeader.timestamp << std::endl;
        }

        // 4. JSON 직렬화 테스트
        nlohmann::json videoJson = videoHeader.ToJson();
        std::cout << "Video Header as JSON:\n" << videoJson.dump(4) << std::endl;

        video::VideoHeader newVideoHeader;
        newVideoHeader.FromJson(videoJson);
        std::cout << "Deserialized Video Header:\n"
                  << "Width: " << newVideoHeader.width
                  << ", Height: " << newVideoHeader.height
                  << ", Frame Rate: " << newVideoHeader.frameRate << std::endl;

        // 5. 비디오 종료
        storage.FinalizeVideo();
        std::cout << "Video finalized and saved to " << filePath << std::endl;

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
