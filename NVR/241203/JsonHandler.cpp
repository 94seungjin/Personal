#include "JsonHandler.h"
#include <fstream>
#include <iostream>
#include <filesystem> // 디렉토리 생성에 사용

namespace edge {

JsonHandler::JsonHandler(const std::string& storagePath, size_t queueSize)
    : mStoragePath(storagePath), mResultQueue(queueSize) {
    // JsonHandler 생성 시 디렉토리 확인 및 생성
    std::filesystem::create_directories(mStoragePath);
}

void JsonHandler::AddDetectionResult(const DetectionResult& result)
{
    if (mResultQueue.IsFull())
    {
        std::cout << "Queue is full. Oldest result will be removed.\n";
        mResultQueue.Pop(); // 큐가 가득 차면 가장 오래된 데이터를 제거
    }
    mResultQueue.Push(result);
}

DetectionResult JsonHandler::GetLatestResult()
{
    if (mResultQueue.IsEmpty())
    {
        throw std::runtime_error("No detection results in the queue.");
    }
    return mResultQueue.Pop(); // 가장 최신 데이터를 반환
}

void JsonHandler::SaveDetectionResultToFile(const DetectionResult& result)
{
    // 디렉토리 확인 및 생성
    std::filesystem::create_directories(mStoragePath);

    std::string filePath = mStoragePath + "/" + result.timestamp + ".json";

    std::ofstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file for saving: " + filePath);
    }

    nlohmann::json jsonObj;
    jsonObj["timestamp"] = result.timestamp;
    for (const auto& obj : result.objects)
    {
        nlohmann::json objJson = {
            {"className", obj.className},
            {"confidence", obj.confidence},
            {"boundingBox", {{"x", obj.x}, {"y", obj.y}, {"width", obj.width}, {"height", obj.height}}}
        };
        jsonObj["objects"].push_back(objJson);
    }

    file << jsonObj.dump(4);
    file.close();
}

DetectionResult JsonHandler::LoadDetectionResultFromFile(const std::string& timestamp)
{
    std::string filePath = mStoragePath + "/" + timestamp + ".json";

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file for loading: " + filePath);
    }

    nlohmann::json jsonObj;
    file >> jsonObj;

    DetectionResult result;
    result.timestamp = jsonObj["timestamp"].get<std::string>();
    for (const auto& objJson : jsonObj["objects"])
    {
        DetectedObject obj;
        obj.className = objJson["className"].get<std::string>();
        obj.confidence = objJson["confidence"].get<float>();
        obj.x = objJson["boundingBox"]["x"].get<int>();
        obj.y = objJson["boundingBox"]["y"].get<int>();
        obj.width = objJson["boundingBox"]["width"].get<int>();
        obj.height = objJson["boundingBox"]["height"].get<int>();
        result.objects.push_back(obj);
    }

    return result;
}

} // namespace edge
