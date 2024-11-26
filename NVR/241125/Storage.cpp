#include "Storage.h"
#include <iostream>
#include <filesystem>

namespace video
{

using json = nlohmann::json;
namespace fs = std::filesystem;

// FrameHeader JSON 변환 메서드
nlohmann::json FrameHeader::ToJson() const
{
    return
    {
        {"frameTimestamp", frameTimestamp},
        {"frameNumber", frameNumber},
        {"width", width},
        {"height", height},
        {"compression", compression}
    };
}

// VideoHeader JSON 변환 메서드
nlohmann::json VideoHeader::ToJson() const
{
    return
    {
        {"frameEndPointer", frameEndPointer},
        {"totalFrames", totalFrames}
    };
}

// Storage 생성자
Storage::Storage(const std::string& directory, size_t maxQueueSize)
    : mDirectory(directory),
      mHeaderDirectory(directory + "/headers"),
      mBodyDirectory(directory + "/body"),
      mMaxQueueSize(maxQueueSize)
{
    initializeDirectories();
}

// Storage 소멸자
Storage::~Storage()
{
    saveToDisk();
}

void Storage::initializeDirectories()
{
    try
    {
        fs::create_directories(mHeaderDirectory);
        fs::create_directories(mBodyDirectory);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error creating directories: " << e.what() << std::endl;
        throw;
    }
}

std::string Storage::getHeaderFilePath(uint32_t frameNumber) const
{
    return mHeaderDirectory + "/FrameHeader_" + std::to_string(frameNumber) + ".json";
}

std::string Storage::getBodyFilePath(uint32_t frameNumber) const
{
    return mBodyDirectory + "/FrameBody_" + std::to_string(frameNumber) + ".bin";
}

void Storage::AddFrame(const FrameHeader& frameHeader, const FrameBody& frameBody)
{
    // frameEndPointer 계산
    size_t lastEndPointer = mVideoHeaders.empty() ? 0 : mVideoHeaders.back().frameEndPointer;
    size_t newEndPointer = lastEndPointer + frameBody.Size();

    // VideoHeader 생성 및 추가
    VideoHeader videoHeader = {newEndPointer, static_cast<uint32_t>(mFrameHeaders.size() + 1)};
    mVideoHeaders.push_back(videoHeader);

    // 프레임 데이터 추가
    mFrameHeaders.push_back(frameHeader);
    mFrameBodies.push_back(frameBody);

    // 디스크에 저장
    std::string headerFilePath = getHeaderFilePath(frameHeader.frameNumber);
    std::string bodyFilePath = getBodyFilePath(frameHeader.frameNumber);

    try
    {
        // Save FrameHeader to JSON
        std::ofstream headerFile(headerFilePath);
        if (!headerFile)
        {
            throw std::ios_base::failure("Failed to open header file: " + headerFilePath);
        }
        headerFile << frameHeader.ToJson().dump(4);

        // Save FrameBody to binary
        std::ofstream bodyFile(bodyFilePath, std::ios::binary);
        if (!bodyFile)
        {
            throw std::ios_base::failure("Failed to open body file: " + bodyFilePath);
        }
        bodyFile.write(reinterpret_cast<const char*>(frameBody.rawData.data()), frameBody.Size());
    }
    catch (const std::exception& e)
    {
        fs::remove(headerFilePath);
        fs::remove(bodyFilePath);
        std::cerr << "Error storing frame: " << e.what() << std::endl;
    }
}

bool Storage::SeekToFrame(uint32_t frameNumber, size_t& frameStartPointer) const
{
    size_t low = 0, high = mVideoHeaders.size() - 1;

    while (low <= high)
    {
        size_t mid = (low + high) / 2;

        if (mFrameHeaders[mid].frameNumber == frameNumber)
        {
            frameStartPointer = mid == 0 ? 0 : mVideoHeaders[mid - 1].frameEndPointer + 1;
            return true;
        }
        else if (mFrameHeaders[mid].frameNumber < frameNumber)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return false;
}

std::string Storage::RetrieveFrame(uint32_t frameNumber, std::vector<uint8_t>& frameBody) const
{
    std::string bodyFilePath = getBodyFilePath(frameNumber);

    if (!fs::exists(bodyFilePath))
    {
        return "Frame not found.";
    }

    std::ifstream bodyFile(bodyFilePath, std::ios::binary);
    if (!bodyFile)
    {
        return "Failed to open frame body file.";
    }

    frameBody.clear();
    bodyFile.seekg(0, std::ios::end);
    size_t fileSize = bodyFile.tellg();
    bodyFile.seekg(0, std::ios::beg);

    frameBody.resize(fileSize);
    bodyFile.read(reinterpret_cast<char*>(frameBody.data()), fileSize);

    return "Frame retrieved successfully.";
}


void Storage::saveToDisk() const
{
    for (size_t i = 0; i < mFrameHeaders.size(); ++i)
    {
        std::string headerFilePath = getHeaderFilePath(mFrameHeaders[i].frameNumber);
        std::string bodyFilePath = getBodyFilePath(mFrameHeaders[i].frameNumber);

        try
        {
            std::ofstream headerFile(headerFilePath);
            if (!headerFile)
            {
                throw std::ios_base::failure("Failed to open header file: " + headerFilePath);
            }
            headerFile << mFrameHeaders[i].ToJson().dump(4);

            std::ofstream bodyFile(bodyFilePath, std::ios::binary);
            if (!bodyFile)
            {
                throw std::ios_base::failure("Failed to open body file: " + bodyFilePath);
            }
            bodyFile.write(reinterpret_cast<const char*>(mFrameBodies[i].rawData.data()), mFrameBodies[i].Size());
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error saving frame to disk: " << e.what() << std::endl;
        }
    }
}

size_t Storage::GetQueueSize() const
{
    return mFrameHeaders.size();
}

} // namespace video
