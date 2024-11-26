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

    struct FrameHeader
    {
        uint64_t FrameTimestamp;   // 프레임 생성 시간
        uint32_t FrameNumber;      // 프레임 번호
        uint32_t Width;            // 프레임 가로 해상도
        uint32_t Height;           // 프레임 세로 해상도
        uint8_t Compression;       // 압축방식

        std::string ToJson() const; // JSON 형식으로 변환
    };

    struct FrameBody
    {
        std::vector<uint8_t> RawData; // 이진 데이터
    };

    struct VideoHeader
    {
        uint64_t StartTimestamp;   // 비디오 구간 시작 타임스탬프
        uint64_t EndTimestamp;     // 비디오 구간 종료 타임스탬프
        size_t FrameStartPointer;  // 해당 구간의 첫 번째 프레임 번호
        size_t FrameEndPointer;    // 해당 구간의 마지막 프레임 번호
        uint32_t Width;            // 구간의 해상도 (가로)
        uint32_t Height;           // 구간의 해상도 (세로)

        std::string ToJson() const; // JSON 형식으로 변환
    };

    class Storage
    {
    public:
        Storage(const std::string& baseDir, size_t maxQueueSize);
        ~Storage();

        // 데이터 저장 메서드
        bool SaveVideoHeader(const VideoHeader& videoHeader); // 비디오 헤더 저장
        bool SaveFrameHeader(const FrameHeader& frameHeader); // 프레임 헤더 저장
        bool SaveFrameBody(const FrameBody& frameBody);       // 프레임 바디 저장

        // 데이터 검색 메서드
        bool SearchVideoHeader(uint64_t timestamp, VideoHeader& videoHeader); // 비디오 헤더 검색
        bool SearchFrameHeader(uint64_t frameTimestamp, FrameHeader& frameHeader);     // 프레임 헤더 검색
        bool SearchFrameBody(uint32_t frameNumber, FrameBody& frameBody);         // 프레임 바디 검색
    
        // 데이터 전송 메서드(to Server)
        bool SendVideoHeader(const VideoHeader& videoHeader); // 서버로 비디오 헤더 전송
        bool SendFrameHeader(const FrameHeader& frameHeader); // 서버로 프레임 헤더 전송
        bool SendFrameBody(const FrameBody& frameBody);       // 서버로 프레임 바디 전송
        
        // 데이터 관리 메서드
        void ClearOldData(); // 오래된 데이터 삭제
        
        // 시스템 상태 반환 메서드
        std::string GetStatus() const;

    private:
        // 디렉토리 및 설정 변수
        std::string mDir;               // 기본 경로
        std::string mVideoHeaderDir;    // 비디오 헤더 저장 폴더 경로
        std::string mFrameHeaderDir;    // 프레임 헤더 저장 폴더 경로
        std::string mFrameBodyDir;      // 프레임 바디 저장 폴더 경로
        size_t mMaxQueueSize;           // 큐의 최대 크기

        // 데이터 저장용 큐
        std::deque<VideoHeader> mVideoHeaders; // 비디오 헤더 큐
        std::deque<FrameHeader> mFrameHeaders; // 프레임 헤더 큐
        std::deque<FrameBody> mFrameBodies;    // 프레임 바디 큐

        // 내부 메서드
        void initDir(); // 디렉토리 초기화
    };

} // namespace video

#endif // STORAGE_H
