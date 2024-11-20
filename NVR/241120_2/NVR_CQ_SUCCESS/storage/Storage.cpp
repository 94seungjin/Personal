#include "Storage.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// Storage 생성자
Storage::Storage(size_t maxSize, const std::string& metadataDir, const std::string& rawdataDir)
    : maxSize(maxSize), metadataDir(metadataDir), rawdataDir(rawdataDir) {
    if (!fs::exists(metadataDir)) fs::create_directories(metadataDir);
    if (!fs::exists(rawdataDir)) fs::create_directories(rawdataDir);
    std::cout << "Storage initialized: " << metadataDir << ", " << rawdataDir << std::endl;
}

// 타임스탬프 생성 함수
std::string Storage::generateTimestamp(int frameNumber) const {
    return "frame_" + std::to_string(frameNumber);
}

// 오래된 데이터 삭제 (Circular Queue)
void Storage::deleteOldData() {
    if (!metadataQueue.empty()) {
        std::cout << "Deleted oldest metadata: " << metadataQueue.front().timestamp << std::endl;
        metadataQueue.pop_front();
    }
    if (!rawDataQueue.empty()) {
        std::cout << "Deleted oldest raw data: " << rawDataQueue.front().timestamp << std::endl;
        rawDataQueue.pop_front();
    }
}

// 메타데이터 저장
bool Storage::saveMetadata(const Metadata& metadata) {
    if (metadataQueue.size() >= maxSize) deleteOldData();
    metadataQueue.push_back(metadata);

    std::string filename = generateFilename(metadataDir, metadata.timestamp, ".bin");
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to save metadata to file: " << filename << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(&metadata.frameWidth), sizeof(metadata.frameWidth));
    file.write(reinterpret_cast<const char*>(&metadata.frameHeight), sizeof(metadata.frameHeight));
    file.write(reinterpret_cast<const char*>(&metadata.totalFrames), sizeof(metadata.totalFrames));
    file.close();

    std::cout << "Saved metadata: " << metadata.timestamp << std::endl;
    return true;
}

// 원본 데이터 저장
bool Storage::saveRawData(const RawData& rawdata) {
    if (rawDataQueue.size() >= maxSize) deleteOldData();
    rawDataQueue.push_back(rawdata);

    std::string filename = generateFilename(rawdataDir, rawdata.timestamp, ".bin");
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to save raw data to file: " << filename << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(rawdata.data.data()), rawdata.data.size());
    file.close();

    std::cout << "Saved raw data: " << rawdata.timestamp << std::endl;
    return true;
}

// 메타데이터 로드
Metadata Storage::loadMetadata(const std::string& timestamp) const {
    auto it = std::find_if(metadataQueue.begin(), metadataQueue.end(),
                           [&timestamp](const Metadata& meta) { return meta.timestamp == timestamp; });
    if (it == metadataQueue.end()) {
        throw std::runtime_error("Metadata not found: " + timestamp);
    }
    return *it;
}

// 원본 데이터 로드
RawData Storage::loadRawData(const std::string& timestamp) const {
    auto it = std::find_if(rawDataQueue.begin(), rawDataQueue.end(),
                           [&timestamp](const RawData& raw) { return raw.timestamp == timestamp; });
    if (it == rawDataQueue.end()) {
        throw std::runtime_error("Raw data not found: " + timestamp);
    }
    return *it;
}

// 파일명 생성
std::string Storage::generateFilename(const std::string& dir, const std::string& timestamp, const std::string& extension) const {
    return dir + "/" + timestamp + extension;
}

// Metadata 및 RawData 추출
bool Storage::extractMetadataAndRawData(const std::string& videoPath) {
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open video file: " << videoPath << std::endl;
        return false;
    }

    Metadata meta;
    meta.frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    meta.frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    meta.totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    meta.timestamp = "Video_Start";

    saveMetadata(meta);

    int frameNumber = 0;
    cv::Mat frame;
    while (cap.read(frame)) {
        std::vector<uint8_t> buffer;
        cv::imencode(".jpg", frame, buffer);

        RawData raw;
        raw.timestamp = generateTimestamp(frameNumber);
        raw.data = buffer;

        saveRawData(raw);
        frameNumber++;
    }

    std::cout << "Extracted " << frameNumber << " frames." << std::endl;
    return true;
}

