#include "Storage.h"
#include <iostream>

namespace video
{
    Storage::Storage()
    {
        initializeDir();
    }

    void Storage::initializeDir()
    {
        try
        {
            fs::create_directories(headerDir);
            fs::create_directories(dataDir);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error creating directories: " << e.what() << std::endl;
            throw;
        }
    }

    void Storage::removeOldestData()
    {
        if (!videoHeaderDeque.empty() && !videoDataDeque.empty())
        {
            const auto& oldestHeader = videoHeaderDeque.front();
            std::string headerFile = headerDir + "/cam01_VideoHeader_" + std::to_string(oldestHeader.frameNumber) + ".json";
            fs::remove(headerFile);

            const auto& oldestDataFile = videoDataDeque.front();
            fs::remove(oldestDataFile);

            videoHeaderDeque.pop_front();
            videoDataDeque.pop_front();
        }
    }

    void Storage::ExtractAndStoreFrames(const std::string& videoPath)
    {
        cv::VideoCapture cap(videoPath);
        if (!cap.isOpened())
        {
            std::cerr << "Error: Unable to open video file: " << videoPath << std::endl;
            return;
        }

        int frameNumber = 0;
        cv::Mat frame;

        while (cap.read(frame))
        {
            VideoHeader header;
            header.videoTimestamp = std::to_string(static_cast<int>(cap.get(cv::CAP_PROP_POS_MSEC))) + "ms";
            header.frameNumber = frameNumber++;
            header.resolution = frame.size();
            header.compression = "JPEG";

            std::vector<uchar> buffer;
            std::vector<int> compressionParams = {cv::IMWRITE_JPEG_QUALITY, 90};
            cv::imencode(".jpg", frame, buffer, compressionParams);

            VideoData data;
            data.rawData = buffer;

            std::string headerFileName = headerDir + "/cam01_VideoHeader_" + std::to_string(header.frameNumber) + ".json";
            std::string dataFileName = dataDir + "/cam01_VideoData_" + std::to_string(header.frameNumber) + ".jpg";

            std::ofstream headerFile(headerFileName);
            headerFile << header.toJson().dump(4);
            headerFile.close();

            std::ofstream dataFile(dataFileName, std::ios::binary);
            dataFile.write(reinterpret_cast<const char*>(data.rawData.data()), data.rawData.size());
            dataFile.close();

            videoHeaderDeque.push_back(header);
            videoDataDeque.push_back(dataFileName);

            if (videoHeaderDeque.size() > maxQueueSize)
            {
                removeOldestData();
            }
        }
        cap.release();
    }

    std::pair<VideoHeader, std::vector<uchar>> Storage::readFrame(int frameNumber)
    {
        std::string headerFileName = headerDir + "/cam01_VideoHeader_" + std::to_string(frameNumber) + ".json";
        if (!fs::exists(headerFileName))
        {
            throw std::runtime_error("Header file not found: " + headerFileName);
        }

        std::ifstream headerFile(headerFileName);
        json headerJson;
        headerFile >> headerJson;
        headerFile.close();

        VideoHeader header;
        header.videoTimestamp = headerJson["videoTimestamp"];
        header.frameNumber = headerJson["frameNumber"];
        header.resolution = cv::Size(headerJson["resolution"][0], headerJson["resolution"][1]);
        header.compression = headerJson["compression"];

        std::string dataFileName = dataDir + "/cam01_VideoData_" + std::to_string(frameNumber) + ".jpg";
        if (!fs::exists(dataFileName))
        {
            throw std::runtime_error("Data file not found: " + dataFileName);
        }

        std::ifstream dataFile(dataFileName, std::ios::binary);
        std::vector<uchar> data((std::istreambuf_iterator<char>(dataFile)), std::istreambuf_iterator<char>());
        dataFile.close();

        return {header, data};
    }

    const std::deque<VideoHeader>& Storage::getVideoHeaders() const
    {
        return videoHeaderDeque;
    }
}

