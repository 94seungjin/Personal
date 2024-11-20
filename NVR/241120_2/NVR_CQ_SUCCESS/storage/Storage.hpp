#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include <vector>
#include <deque>
#include <opencv2/opencv.hpp>

struct Metadata
{
    std::string timestamp; // 타임스탬프
    int frameWidth;        // 프레임 너비
    int frameHeight;       // 프레임 높이
    int totalFrames;       // 총 프레임 수
};

struct RawData
{
    std::string timestamp;    // 타임스탬프
    std::vector<uint8_t> data; // 인코딩된 원본 데이터
};

class Storage
{
public:
    Storage(size_t maxSize, const std::string& metadataDir, const std::string& rawdataDir);
    bool extractMetadataAndRawData(const std::string& videoPath); // MP4에서 추출
    bool saveMetadata(const Metadata& metadata);
    bool saveRawData(const RawData& rawdata);
    Metadata loadMetadata(const std::string& timestamp) const;
    RawData loadRawData(const std::string& timestamp) const;

private:
    size_t maxSize;                        // Circular Queue 최대 크기
    std::deque<Metadata> metadataQueue;    // Circular Queue (메타데이터)
    std::deque<RawData> rawDataQueue;      // Circular Queue (원본 데이터)
    std::string metadataDir;               // 메타데이터 저장 경로
    std::string rawdataDir;                // 원본 데이터 저장 경로

    void deleteOldData();                  // 오래된 데이터 삭제
    std::string generateFilename(const std::string& dir, const std::string& timestamp, const std::string& extension) const;
    std::string generateTimestamp(int frameNumber) const;
};

#endif // STORAGE_HPP

