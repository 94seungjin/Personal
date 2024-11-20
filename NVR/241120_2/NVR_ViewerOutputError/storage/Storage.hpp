#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include <vector>
#include <deque>

// Metadata 구조체
struct Metadata
{
    std::string timestamp;  // 타임스탬프
    int frameWidth;         // 프레임 너비
    int frameHeight;        // 프레임 높이
    int totalFrames;        // 총 프레임 수
};

// RawData 구조체
struct RawData
{
    std::string timestamp;  // 타임스탬프
    std::vector<uint8_t> data; // 프레임 데이터
};

// Storage 클래스
class Storage
{
public:
    Storage(size_t maxSize, const std::string& metadataDir, const std::string& rawdataDir);

    // 메타데이터와 RawData 저장
    bool saveMetadata(const Metadata& metadata);
    bool saveRawData(const RawData& rawData);

    // 메타데이터와 RawData 로드
    Metadata loadMetadata(const std::string& timestamp) const;
    RawData loadRawData(const std::string& timestamp) const;

private:
    size_t maxSize;                          // Circular Queue 최대 크기
    std::string metadataDir;                 // 메타데이터 저장 디렉토리
    std::string rawdataDir;                  // 원본 데이터 저장 디렉토리
    std::deque<Metadata> metadataQueue;      // 메타데이터 Circular Queue
    std::deque<RawData> rawDataQueue;        // RawData Circular Queue

    void deleteOldData();                    // 오래된 데이터 삭제
    std::string generateFilename(const std::string& dir, const std::string& timestamp, const std::string& extension) const;
};

#endif // STORAGE_HPP

