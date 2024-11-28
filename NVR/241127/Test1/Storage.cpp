#include "Storage.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace video {

Storage::Storage(const std::string& filePath, size_t maxFrames)
    : mFilePath(filePath), mFrameQueue(maxFrames), mCurrentFrameId(0) {}

Storage::~Storage()
{
    if (mFileStream.is_open())
    {
        mFileStream.close();
    }
    if (mFileInputStream.is_open())
    {
        mFileInputStream.close();
    }
}

void Storage::InitializeVideo(const VideoHeader& videoHeader)
{
    mVideoHeader = videoHeader;

    mFileStream.open(mFilePath, std::ios::binary | std::ios::trunc);
    if (!mFileStream)
    {
        throw std::runtime_error("Failed to open file for writing: " + mFilePath);
    }

    std::vector<uint8_t> headerBuffer;
    mVideoHeader.Serialize(headerBuffer);

    mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
    if (!mFileStream)
    {
        throw std::runtime_error("Failed to write VideoHeader to file.");
    }

    mFileStream.flush();
}

void Storage::SaveFrame(const FrameHeader& header, const FrameBody& body)
{
    if (!mFileStream.is_open() || mFileStream.fail() || mFileStream.bad())
    {
        mFileStream.open(mFilePath, std::ios::binary | std::ios::app);
        if (!mFileStream)
        {
            throw std::runtime_error("Failed to reopen file stream.");
        }
    }

    mFrameQueue.Push(header, body);
    writeFrameToFile(header, body);

    mCurrentFrameId++;
    mVideoHeader.totalFrames = mCurrentFrameId;
}

bool Storage::GetNextFrame(FrameHeader& header, FrameBody& body)
{
    if (mFrameQueue.IsEmpty())
    {
        return false;
    }

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
    if (!mFileStream.is_open())
    {
        throw std::runtime_error("File stream is not open for writing.");
    }

    std::vector<uint8_t> headerBuffer;
    header.Serialize(headerBuffer);
    mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
    if (mFileStream.fail())
    {
        throw std::runtime_error("Failed to write FrameHeader to file.");
    }

    std::vector<uint8_t> bodyBuffer;
    body.Serialize(bodyBuffer);
    mFileStream.write(reinterpret_cast<const char*>(bodyBuffer.data()), bodyBuffer.size());
    if (mFileStream.fail())
    {
        throw std::runtime_error("Failed to write FrameBody to file.");
    }

    mFileStream.flush();
}

} // namespace video
