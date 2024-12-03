#include <iostream>
#include <vector>
#include <string>
#include "VideoHeader.h"
#include "FrameHeader.h"
#include "FrameBody.h"
#include "JsonHandler.h"
#include "FrameQueue.h"
#include "Serialize.h"
#include "Storage.h"

using namespace video;
using namespace edge;

// Serialize and Deserialize Test
void TestSerialize() {
    std::cout << "\n[1] Testing Serialize and Deserialize..." << std::endl;

    FrameHeader header = {1, 1024, "20231203_123456.789"};
    std::vector<uint8_t> buffer;

    // Serialize
    utils::Serialize(header, buffer);
    std::cout << "Serialized buffer size: " << buffer.size() << std::endl;

    // Deserialize
    FrameHeader deserializedHeader;
    utils::Deserialize(buffer, deserializedHeader);
    std::cout << "Deserialized Frame ID: " << deserializedHeader.frameId
              << ", Body Size: " << deserializedHeader.bodySize
              << ", Timestamp: " << deserializedHeader.timestamp << std::endl;
}

// FrameQueue Test
void TestFrameQueue() {
    std::cout << "\n[2] Testing FrameQueue..." << std::endl;

    FrameQueue<std::pair<FrameHeader, FrameBody>> queue(3);
    FrameHeader header1 = {1, 100, "20231203_120000.000"};
    FrameBody body1 = {{1, 2, 3, 4, 5}};

    FrameHeader header2 = {2, 200, "20231203_120001.000"};
    FrameBody body2 = {{10, 20, 30, 40, 50}};

    // Push data into the queue
    queue.Push({header1, body1});
    queue.Push({header2, body2});

    // Pop data and validate
    auto frame = queue.Pop();
    std::cout << "Popped Frame ID: " << frame.first.frameId
              << ", Data Size: " << frame.second.data.size() << std::endl;
}

// JsonHandler Test
void TestJsonHandler() {
    std::cout << "\n[3] Testing JsonHandler..." << std::endl;

    JsonHandler jsonHandler("./json_storage", 3);

    DetectionResult result1 = {
        "20231203T120000",
        {
            {"person", 0.95, 10, 20, 30, 40},
            {"car", 0.85, 50, 60, 70, 80}
        }
    };

    DetectionResult result2 = {
        "20231203T120001",
        {
            {"cat", 0.90, 15, 25, 35, 45}
        }
    };

    // Add detection results
    jsonHandler.AddDetectionResult(result1);
    jsonHandler.AddDetectionResult(result2);

    // Save and load JSON
    jsonHandler.SaveDetectionResultToFile(result1);
    jsonHandler.SaveDetectionResultToFile(result2);

    auto loadedResult = jsonHandler.LoadDetectionResultFromFile("20231203T120000");
    std::cout << "Loaded JSON Timestamp: " << loadedResult.timestamp
              << ", Object Count: " << loadedResult.objects.size() << std::endl;
}

// Storage Test
void TestStorage() {
    std::cout << "\n[4] Testing Storage..." << std::endl;

    Storage storage("./video_storage.bin", 3);

    VideoHeader videoHeader = {0, 1920, 1080, 30, "20231203_120000", {}, 0, 0};
    storage.InitializeVideo(videoHeader);

    FrameHeader header1 = {1, 100, "20231203_120000.000"};
    FrameBody body1 = {{1, 2, 3, 4, 5}};

    FrameHeader header2 = {2, 200, "20231203_120001.000"};
    FrameBody body2 = {{10, 20, 30, 40, 50}};

    storage.SaveFrame(header1, body1);
    storage.SaveFrame(header2, body2);

    FrameHeader retrievedHeader;
    FrameBody retrievedBody;
    while (storage.GetNextFrame(retrievedHeader, retrievedBody)) {
        std::cout << "Retrieved Frame ID: " << retrievedHeader.frameId
                  << ", Body Size: " << retrievedBody.data.size() << std::endl;
    }

    storage.FinalizeVideo();
}

// VideoHeader Test
void TestVideoHeader() {
    std::cout << "\n[5] Testing VideoHeader..." << std::endl;

    VideoHeader header = {100, 1920, 1080, 30, "20231203_120000", {}, 0, 1000};
    std::vector<uint8_t> buffer;

    // Serialize
    header.Serialize(buffer);
    std::cout << "Serialized VideoHeader buffer size: " << buffer.size() << std::endl;

    // Deserialize
    VideoHeader deserializedHeader;
    deserializedHeader.Deserialize(buffer);
    std::cout << "Deserialized Video Total Frames: " << deserializedHeader.totalFrames
              << ", Resolution: " << deserializedHeader.width << "x" << deserializedHeader.height
              << ", Frame Rate: " << deserializedHeader.frameRate << std::endl;
}

int main() {
    try {
        TestSerialize();
        TestFrameQueue();
        TestJsonHandler();
        TestStorage();
        TestVideoHeader();
        std::cout << "\nAll tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed: " << e.what() << std::endl;
    }

    return 0;
}
