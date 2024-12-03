
<클래스<		<역할<
VideoHeader	비디오의 메타데이터를 관리 (총 프레임 수, 해상도, 프레임 레이트 등).
FrameHeader	개별 프레임의 메타데이터를 저장 (ID, 크기, 타임스탬프).
FrameBody	프레임의 실제 데이터를 저장
FrameQueue	원형 큐를 구현하여 데이터 순환 관리를 지원
Serialize	객체를 바이너리로 직렬화하거나, 바이너리 데이터를 객체로 역직렬화
JsonHandler	JSON 데이터를 관리 (탐지 결과 저장, 큐 관리, 파일 저장 및 로드).
Storage		비디오 메타데이터, 프레임 데이터를 파일로 저장하거나 로드

<JSON 데이터 처리>
#include "JsonHandler.h"

int main()
{
    edge::JsonHandler jsonHandler("./json_storage", 5);

    // 탐지 결과 추가
    edge::DetectionResult result = {
        "20231203T120000",
        {
            {"person", 0.95, 10, 20, 30, 40},
            {"car", 0.85, 50, 60, 70, 80}
        }
    };
    jsonHandler.AddDetectionResult(result);

    // 파일로 저장
    jsonHandler.SaveDetectionResultToFile(result);

    // 파일에서 로드
    auto loadedResult = jsonHandler.LoadDetectionResultFromFile("20231203T120000");
    std::cout << "Loaded JSON Timestamp: " << loadedResult.timestamp << std::endl;

    return 0;
}


<비디오 데이터 처리>
#include "Storage.h"
#include "FrameHeader.h"
#include "FrameBody.h"
#include "VideoHeader.h"

int main()
{
    video::Storage storage("./video_storage.bin", 5);

    // 비디오 초기화
    video::VideoHeader videoHeader = {0, 1920, 1080, 30, "20231203_120000", {}, 0, 0};
    storage.InitializeVideo(videoHeader);

    // 프레임 저장
    video::FrameHeader frameHeader = {1, 100, "20231203_120000.000"};
    video::FrameBody frameBody = {{1, 2, 3, 4, 5}};
    storage.SaveFrame(frameHeader, frameBody);

    // 저장된 프레임 로드
    video::FrameHeader retrievedHeader;
    video::FrameBody retrievedBody;
    if (storage.GetNextFrame(retrievedHeader, retrievedBody))
    {
        std::cout << "Retrieved Frame ID: " << retrievedHeader.frameId << std::endl;
    }

    storage.FinalizeVideo();
    return 0;
}
