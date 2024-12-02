#include "Storage.h"
#include <stdexcept>

namespace video {

Storage::Storage(const std::string& filePath, size_t maxFrames)
    : mFilePath(filePath), mFrameQueue(maxFrames), mCurrentFrameId(0) {}

Storage::~Storage()
{
    if (mFileStream.is_open())
        mFileStream.close();
}

void Storage::InitializeVideo(const VideoHeader& videoHeader)
{
    mVideoHeader = videoHeader;
    mFileStream.open(mFilePath, std::ios::binary | std::ios::trunc);
    if (!mFileStream)
        throw std::runtime_error("Failed to open file for writing.");

    std::vector<uint8_t> headerBuffer;
    mVideoHeader.Serialize(headerBuffer);
    mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
    mFileStream.flush();
}

void Storage::SaveFrame(const FrameHeader& header, const FrameBody& body)
{
    mFrameQueue.Push(header, body);
    writeFrameToFile(header, body);
    mCurrentFrameId++;
    mVideoHeader.totalFrames = mCurrentFrameId;
}

bool Storage::GetNextFrame(FrameHeader& header, FrameBody& body)
{
    if (mFrameQueue.IsEmpty())
        return false;

    auto frame = mFrameQueue.Pop();
    header = frame.first;
    body = frame.second;
    return true;
}

void Storage::FinalizeVideo()
{
    if (mFileStream.is_open())
    {
        mVideoHeader.endPoint = mFileStream.tellp();
        mFileStream.seekp(0);

        std::vector<uint8_t> headerBuffer;
        mVideoHeader.Serialize(headerBuffer);
        mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
        mFileStream.flush();
        mFileStream.close();
    }
}

void Storage::writeFrameToFile(const FrameHeader& header, const FrameBody& body)
{
    std::vector<uint8_t> headerBuffer;
    header.Serialize(headerBuffer);
    mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());

    std::vector<uint8_t> bodyBuffer;
    body.Serialize(bodyBuffer);
    mFileStream.write(reinterpret_cast<const char*>(bodyBuffer.data()), bodyBuffer.size());

    mFileStream.flush();
}

} // namespace video
