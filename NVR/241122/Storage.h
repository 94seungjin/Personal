#ifndef STORAGE_H
#define STORAGE_H

#include <opencv2/opencv.hpp>
#include <deque>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>

namespace video
{
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    struct VideoHeader
    {
        std::string videoTimestamp;
        int frameNumber;
        cv::Size resolution;
        std::string compression;

        json toJson() const
        {
            return
            {
                {"videoTimestamp", videoTimestamp},
                {"frameNumber", frameNumber},
                {"resolution", {resolution.width, resolution.height}},
                {"compression", compression}
            };
        }
    };

    struct VideoData
    {
        std::vector<uchar> rawData;
    };

    class Storage
    {
    private:
        std::deque<VideoHeader> videoHeaderDeque;
        std::deque<std::string> videoDataDeque;

        const size_t maxQueueSize = 100;

        const std::string baseDir = "storage";
        const std::string headerDir = "storage/videoheader";
        const std::string dataDir = "storage/videodata";

        void initializeDir();
        void removeOldestData();

    public:
        Storage();

        void ExtractAndStoreFrames(const std::string& videoPath);
        std::pair<VideoHeader, std::vector<uchar>> readFrame(int frameNumber);
        const std::deque<VideoHeader>& getVideoHeaders() const;
    };
}

#endif // STORAGE_H

