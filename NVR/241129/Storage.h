#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <fstream>
#include "VideoHeader.h"
#include "FrameQueue.h"

namespace video {

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
    FrameQueue mFrameQueue;
    VideoHeader mVideoHeader;
    uint32_t mCurrentFrameId;

    void writeFrameToFile(const FrameHeader& header, const FrameBody& body);
};

} // namespace video

#endif // STORAGE_H
