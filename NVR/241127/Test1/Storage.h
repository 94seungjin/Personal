#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <iostream>

namespace video
{

struct VideoHeader
{
    uint32_t totalFrames;
    uint16_t width;
    uint16_t height;
    uint32_t frameRate;
    char creationTime[19];
    uint8_t padding1[1];
    uint64_t startPoint;
    uint64_t endPoint;

    void Serialize(std::vector<uint8_t>& buffer) const
    {
        buffer.resize(sizeof(VideoHeader));
        std::memcpy(buffer.data(), this, sizeof(VideoHeader));
    }

    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        if (buffer.size() < sizeof(VideoHeader))
        {
            throw std::runtime_error("Buffer size is too small to deserialize VideoHeader");
        }
        std::memcpy(this, buffer.data(), sizeof(VideoHeader));
    }
};

struct FrameHeader
{
    uint32_t frameId;
    uint32_t bodySize;
    char timestamp[19];

    void Serialize(std::vector<uint8_t>& buffer) const
    {
        buffer.resize(sizeof(FrameHeader));
        std::memcpy(buffer.data(), this, sizeof(FrameHeader));
    }

    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        if (buffer.size() < sizeof(FrameHeader))
        {
            throw std::runtime_error("Buffer size is too small to deserialize FrameHeader");
        }
        std::memcpy(this, buffer.data(), sizeof(FrameHeader));
    }
};

struct FrameBody
{
    std::vector<uint8_t> data;

    void Serialize(std::vector<uint8_t>& buffer) const
    {
        buffer = data;
    }

    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        data = buffer;
    }
};

class FrameQueue
{
public:
    explicit FrameQueue(size_t capacity)
        : mCapacity(capacity), mHead(0), mTail(0), mSize(0), mQueue(capacity) {}

    bool IsEmpty() const { return mSize == 0; }
    bool IsFull() const { return mSize == mCapacity; }

    void Push(const FrameHeader& header, const FrameBody& body)
    {
        mQueue[mTail] = {header, body};
        mTail = (mTail + 1) % mCapacity;
        if (IsFull())
        {
            mHead = (mHead + 1) % mCapacity;
        }
        else
        {
            mSize++;
        }
    }

    std::pair<FrameHeader, FrameBody> Pop()
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Queue is empty");
        }
        auto frame = mQueue[mHead];
        mHead = (mHead + 1) % mCapacity;
        mSize--;
        return frame;
    }

private:
    size_t mCapacity;
    size_t mHead;
    size_t mTail;
    size_t mSize;
    std::vector<std::pair<FrameHeader, FrameBody>> mQueue;
};

class Storage
{
public:
    Storage(const std::string& filePath, size_t maxFrames);
    ~Storage();

    void InitializeVideo(const VideoHeader& videoHeader);
    void SaveFrame(const FrameHeader& header, const FrameBody& body);
    bool GetNextFrame(FrameHeader& header, FrameBody& body);
    void FinalizeVideo();

private:
    std::string mFilePath;
    std::ofstream mFileStream;
    std::ifstream mFileInputStream;
    FrameQueue mFrameQueue;

    VideoHeader mVideoHeader;
    uint32_t mCurrentFrameId;

    void writeFrameToFile(const FrameHeader& header, const FrameBody& body);
};

} // namespace video

#endif // STORAGE_H
