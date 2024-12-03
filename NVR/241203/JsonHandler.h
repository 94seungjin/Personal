#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "FrameQueue.h"

namespace edge {

struct DetectedObject
{
    std::string className;
    float confidence;
    int x;
    int y;
    int width;
    int height;
};

struct DetectionResult
{
    std::string timestamp;
    std::vector<DetectedObject> objects;
};

class JsonHandler {
public:
    JsonHandler(const std::string& storagePath, size_t queueSize);
    ~JsonHandler() = default;

    void AddDetectionResult(const DetectionResult& result); // 탐지 결과 추가
    DetectionResult GetLatestResult();                      // 최신 탐지 결과 반환
    void SaveDetectionResultToFile(const DetectionResult& result);
    DetectionResult LoadDetectionResultFromFile(const std::string& timestamp);

private:
    std::string mStoragePath;                 // JSON 데이터를 저장할 디렉토리
    FrameQueue<DetectionResult> mResultQueue; // 원형 큐로 탐지 결과 관리
};

} // namespace edge

#endif // JSON_HANDLER_H
