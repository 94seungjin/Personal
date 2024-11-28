#include "Storage.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>

void TestVideoHeaderSerialization();
void TestInitializeVideo();
void TestSaveFrame();
void TestGetNextFrame();

int main()
{
    TestVideoHeaderSerialization();
    TestInitializeVideo();
    TestSaveFrame();
    TestGetNextFrame();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

void TestVideoHeaderSerialization()
{
    std::cout << "\n[TEST] VideoHeader Serialization\n";

    // Before: Create VideoHeader
    video::VideoHeader originalHeader = {10, 1920, 1080, 30, "", 0, 1000, 0};
    std::strcpy(originalHeader.creationTime, "20231126_123456");

    std::cout << "Before Serialization:\n";
    std::cout << "  TotalFrames: " << originalHeader.totalFrames
              << ", Width: " << originalHeader.width
              << ", Height: " << originalHeader.height
              << ", FrameRate: " << originalHeader.frameRate
              << ", CreationTime: " << originalHeader.creationTime
              << ", StartPoint: " << originalHeader.startPoint
              << ", EndPoint: " << originalHeader.endPoint << "\n";

    // Serialize and Deserialize
    std::vector<uint8_t> buffer;
    originalHeader.Serialize(buffer);
    video::VideoHeader deserializedHeader;
    deserializedHeader.Deserialize(buffer);

    // After: Check deserialized values
    std::cout << "After Deserialization:\n";
    std::cout << "  TotalFrames: " << deserializedHeader.totalFrames
              << ", Width: " << deserializedHeader.width
              << ", Height: " << deserializedHeader.height
              << ", FrameRate: " << deserializedHeader.frameRate
              << ", CreationTime: " << deserializedHeader.creationTime
              << ", StartPoint: " << deserializedHeader.startPoint
              << ", EndPoint: " << deserializedHeader.endPoint << "\n";

    // Assertions
    assert(deserializedHeader.totalFrames == originalHeader.totalFrames);
    assert(deserializedHeader.width == originalHeader.width);
    assert(deserializedHeader.height == originalHeader.height);
    assert(deserializedHeader.frameRate == originalHeader.frameRate);
    assert(std::string(deserializedHeader.creationTime) == std::string(originalHeader.creationTime));
    assert(deserializedHeader.startPoint == originalHeader.startPoint);
    assert(deserializedHeader.endPoint == originalHeader.endPoint);

    std::cout << "[PASSED] VideoHeader Serialization\n";
}

void TestInitializeVideo()
{
    video::Storage storage("test_video.dat", 10);

    video::VideoHeader header = {0, 1920, 1080, 30, "", 0, 0, 0};
    std::strncpy(header.creationTime, "20231126_123456", sizeof(header.creationTime) - 1);
    header.creationTime[sizeof(header.creationTime) - 1] = '\0';

    storage.InitializeVideo(header);

    std::ifstream file("test_video.dat", std::ios::binary);
    assert(file.is_open());

    std::vector<uint8_t> buffer(sizeof(video::VideoHeader));
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    std::cout << "[DEBUG] Serialized Data (Read from File): ";
    for (uint8_t byte : buffer)
    {
        printf("%02X ", byte);
    }
    std::cout << std::endl;

    video::VideoHeader readHeader;
    readHeader.Deserialize(buffer);

    std::cout << "[DEBUG] Deserialized Header:\n"
              << "  Width: " << readHeader.width << "\n"
              << "  Height: " << readHeader.height << "\n"
              << "  FrameRate: " << readHeader.frameRate << "\n"
              << "  CreationTime: " << readHeader.creationTime << "\n";

    assert(readHeader.width == 1920);
    assert(readHeader.height == 1080);
    assert(readHeader.frameRate == 30);
    assert(std::string(readHeader.creationTime) == "20231126_123456");
}


void TestSaveFrame()
{
    video::Storage storage("test_video.dat", 10);

    // FrameHeader와 FrameBody 생성
    video::FrameHeader frameHeader = {1, 5, ""};
    strcpy(frameHeader.timestamp, "20231126_123457.001");
    video::FrameBody frameBody = {{1, 2, 3, 4, 5}};

    std::cout << "Before Saving Frame:\n";
    std::cout << "  FrameID: " << frameHeader.frameId
              << ", BodySize: " << frameHeader.bodySize
              << ", Timestamp: " << frameHeader.timestamp << "\n";

    storage.SaveFrame(frameHeader, frameBody);

    // After: 파일에서 데이터 읽기
    std::ifstream file("test_video.dat", std::ios::binary);
    file.seekg(sizeof(video::VideoHeader), std::ios::beg);

    std::cout << "File pointer position after seekg: " << file.tellg() << std::endl;

    std::vector<uint8_t> buffer(sizeof(video::FrameHeader));
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    video::FrameHeader readHeader;
    readHeader.Deserialize(buffer);

    std::cout << "After Saving Frame:\n";
    std::cout << "  FrameID: " << readHeader.frameId
              << ", BodySize: " << readHeader.bodySize
              << ", Timestamp: " << readHeader.timestamp << "\n";

    std::vector<uint8_t> bodyBuffer(5);
    file.read(reinterpret_cast<char*>(bodyBuffer.data()), 5);

    std::cout << "Body Data After Saving Frame: ";
    for (uint8_t byte : bodyBuffer)
    {
        printf("%02X ", byte);
    }
    std::cout << std::endl;

    file.close();

    // Assertions
    assert(readHeader.frameId == 1);
    assert(readHeader.bodySize == 5);
    assert(std::string(readHeader.timestamp) == "20231126_123457.001");
    assert(bodyBuffer == std::vector<uint8_t>({1, 2, 3, 4, 5}));

    std::cout << "[PASSED] Save Frame\n";
}




void TestGetNextFrame()
{
    std::cout << "\n[TEST] Get Next Frame\n";

    video::Storage storage("test_video.dat", 10);

    // Before: Save a frame to queue
    video::FrameHeader frameHeader = {1, 5, ""};
    std::strcpy(frameHeader.timestamp, "20231126_123457.001");
    video::FrameBody frameBody = {{1, 2, 3, 4, 5}};
    storage.SaveFrame(frameHeader, frameBody);

    // After: Retrieve frame from queue
    video::FrameHeader outHeader;
    video::FrameBody outBody;
    assert(storage.GetNextFrame(outHeader, outBody));

    std::cout << "After Retrieving Frame:\n";
    std::cout << "  FrameID: " << outHeader.frameId
              << ", BodySize: " << outHeader.bodySize
              << ", Timestamp: " << outHeader.timestamp << "\n";
    std::cout << "  Body Data: ";
    for (uint8_t byte : outBody.data)
    {
        printf("%02X ", byte);
    }
    std::cout << "\n";

    // Assertions
    assert(outHeader.frameId == 1);
    assert(outHeader.bodySize == 5);
    assert(std::string(outHeader.timestamp) == "20231126_123457.001");
    assert(outBody.data == std::vector<uint8_t>({1, 2, 3, 4, 5}));

    std::cout << "[PASSED] Get Next Frame\n";
}
