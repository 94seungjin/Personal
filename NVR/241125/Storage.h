#ifndef STORAGE_H
#define STORAGE_H

#include <deque>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace video
{

    // FrameHeader 구조체
    struct FrameHeader
    {
        uint64_t frameTimestamp;   // 프레임 생성 시간
        uint32_t frameNumber;      // 프레임 고유 번호
        uint16_t width;            // 프레임 가로 해상도
        uint16_t height;           // 프레임 세로 해상도
        std::string compression;   // 압축 형식 (예: H264)

        // JSON 변환 메서드
        nlohmann::json ToJson() const;
    };

    // FrameBody 구조체
    struct FrameBody
    {
        std::vector<uint8_t> rawData; // 프레임 바이너리 데이터
    
        // 데이터 크기 반환 메서드
        size_t Size() const { return rawData.size(); }
    };

    // VideoHeader 구조체
    struct VideoHeader
    {
        size_t frameEndPointer;   // 마지막 프레임 끝 위치

        // JSON 변환 메서드
        nlohmann::json ToJson() const;
    };

    // Storage 클래스
    class Storage
    {
    public:
        Storage(const std::string& directory, size_t maxQueueSize);
        ~Storage();

        void AddFrame(const FrameHeader& frameHeader, const FrameBody& frameBody);
        bool SeekToFrame(uint32_t frameNumber, size_t& frameStartPointer) const;
        std::string RetrieveFrame(uint32_t frameNumber, std::vector<uint8_t>& frameBody) const;
        bool ValidateData() const;
        void ValidateFrameEndPointers() const;

        size_t GetQueueSize() const;
        size_t GetMaxQueueSize() const;
        void SetMaxQueueSize(size_t maxQueueSize);

    private:
        std::string mDirectory;
        std::string mVideoHeaderDirec   tory;     // 비디오헤더파일 저장 디렉터
        std::string mHeaderDirectory;          // 헤더 파일 저장 디렉터리
        std::string mBodyDirectory;            // 데이터 파일 저장 디렉터리
        size_t mMaxQueueSize;

        std::deque<VideoHeader> mVideoHeaders;
        std::deque<FrameHeader> mFrameHeaders;
        std::deque<FrameBody> mFrameBodies;

        void initializeDirectories();
        std::string getHeaderFilePath(uint32_t frameNumber) const;
        std::string getBodyFilePath(uint32_t frameNumber) const;
        void removeOldestData();
        void saveToDisk() const;
    };
} // namespace video

#endif // STORAGE_H
