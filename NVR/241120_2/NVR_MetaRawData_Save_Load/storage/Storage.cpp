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
}

// 오래된 데이터 삭제
void Storage::deleteOldData() {
    if (!metadataQueue.empty()) metadataQueue.pop_front();
    if (!rawDataQueue.empty()) rawDataQueue.pop_front();
}

// 파일 이름 생성
std::string Storage::generateFilename(const std::string& dir, const std::string& timestamp, const std::string& extension) const {
    return dir + "/" + timestamp + extension;
}

// 메타데이터 저장
bool Storage::saveMetadata(const Metadata& metadata) {
    if (metadataQueue.size() >= maxSize) deleteOldData();
    metadataQueue.push_back(metadata);

    std::string filename = generateFilename(metadataDir, metadata.timestamp, ".bin");
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    file.write(reinterpret_cast<const char*>(&metadata.frameWidth), sizeof(metadata.frameWidth));
    file.write(reinterpret_cast<const char*>(&metadata.frameHeight), sizeof(metadata.frameHeight));
    file.write(reinterpret_cast<const char*>(&metadata.totalFrames), sizeof(metadata.totalFrames));
    return true;
}

// RawData 저장
bool Storage::saveRawData(const RawData& rawData) {
    if (rawDataQueue.size() >= maxSize) deleteOldData();
    rawDataQueue.push_back(rawData);

    std::string filename = generateFilename(rawdataDir, rawData.timestamp, ".bin");
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    file.write(reinterpret_cast<const char*>(rawData.data.data()), rawData.data.size());
    return true;
}

// 메타데이터 로드
Metadata Storage::loadMetadata(const std::string& timestamp) const {
    auto it = std::find_if(metadataQueue.begin(), metadataQueue.end(),
                           [&timestamp](const Metadata& meta) { return meta.timestamp == timestamp; });
    if (it != metadataQueue.end()) return *it;

    std::string filename = generateFilename(metadataDir, timestamp, ".bin");
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Metadata not found for timestamp: " + timestamp);

    Metadata metadata;
    file.read(reinterpret_cast<char*>(&metadata.frameWidth), sizeof(metadata.frameWidth));
    file.read(reinterpret_cast<char*>(&metadata.frameHeight), sizeof(metadata.frameHeight));
    file.read(reinterpret_cast<char*>(&metadata.totalFrames), sizeof(metadata.totalFrames));
    metadata.timestamp = timestamp;
    return metadata;
}

// RawData 로드
RawData Storage::loadRawData(const std::string& timestamp) const {
    auto it = std::find_if(rawDataQueue.begin(), rawDataQueue.end(),
                           [&timestamp](const RawData& raw) { return raw.timestamp == timestamp; });
    if (it != rawDataQueue.end()) return *it;

    std::string filename = generateFilename(rawdataDir, timestamp, ".bin");
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("RawData not found for timestamp: " + timestamp);

    RawData rawData;
    rawData.timestamp = timestamp;

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    rawData.data.resize(size);
    file.read(reinterpret_cast<char*>(rawData.data.data()), size);
    return rawData;
}

