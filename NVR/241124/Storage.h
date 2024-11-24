#ifndef STORAGE_H
#define STORAGE_H

#include <deque>
#include <string>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace video
{
    using json = nlohmann::json;  // JSON 라이브러리 별칭
    namespace fs = std::filesystem;  // 파일 시스템 별칭

    /**
     * FrameHeader
     * - 프레임 메타데이터를 나타내는 구조체
     * - 모든 멤버 변수는 public으로 선언
     * - 구조체 내부에서는 함수 사용 불가
     */
    struct FrameHeader
    {
        std::string Timestamp;        // 프레임 타임스탬프 (생성 시간)
        int FrameNumber;              // 프레임 번호 (고유 식별자)
        int Width;                    // 이미지 너비 (픽셀 단위)
        int Height;                   // 이미지 높이 (픽셀 단위)
        std::string Compression;      // 압축 형식 (예: JPEG, PNG 등)
    };

    /**
     * FrameBody
     * - 실제 프레임 데이터를 저장하는 구조체
     * - 바이너리 형식의 이미지 데이터를 포함
     */
    struct FrameBody
    {
        std::vector<uint8_t> RawData; // 이미지 바이너리 데이터
    };

    /**
     * Storage
     * - 프레임 데이터(헤더 + 바디)를 관리하는 클래스
     * - 데이터를 저장하고 삭제하며, 파일 시스템에 데이터를 유지
     */
    class Storage
    {
    private:
        std::deque<FrameHeader> mHeaderQueue;  // 메모리에서 관리하는 프레임 헤더 큐
        std::deque<std::string> mDataQueue;    // 메모리에서 관리하는 프레임 데이터 파일 경로 큐
        size_t mMaxQueueSize = 100;            // 최대 큐 크기 (기본값: 100)

        const std::string mBaseDir = "storage";       // 데이터 저장 기본 디렉토리
        const std::string mHeaderDir = mBaseDir + "/headers"; // 헤더 파일 저장 디렉토리
        const std::string mDataDir = mBaseDir + "/data";      // 데이터 파일 저장 디렉토리

        /**
         * initializeDirectories
         * - 필요한 디렉토리(헤더, 데이터)를 초기화
         * - 디렉토리가 없으면 생성
         */
        void initializeDirectories();

        /**
         * removeOldestData
         * - 큐 크기를 초과할 경우 가장 오래된 데이터를 삭제
         * - 헤더 및 데이터 파일 모두 삭제
         */
        void removeOldestData();

        /**
         * getHeaderFilePath
         * - 프레임 번호를 기반으로 헤더 파일 경로 생성
         * @param frameNumber 프레임 번호
         * @return 헤더 파일 경로
         */
        std::string getHeaderFilePath(int frameNumber) const;

        /**
         * getDataFilePath
         * - 프레임 번호를 기반으로 데이터 파일 경로 생성
         * @param frameNumber 프레임 번호
         * @return 데이터 파일 경로
         */
        std::string getDataFilePath(int frameNumber) const;

    public:
        Storage(size_t maxQueueSize = 100); // 생성자: 큐 최대 크기 설정 가능
        ~Storage() = default; // 기본 소멸자

        /**
         * StoreFrame
         * - 프레임 데이터를 저장 (헤더 + 바디)
         * @param header 프레임 메타데이터
         * @param body 프레임 바이너리 데이터
         */
        void StoreFrame(const FrameHeader& header, const FrameBody& body);

        /**
         * RetrieveFrame
         * - 특정 프레임 번호의 데이터를 검색
         * @param frameNumber 검색할 프레임 번호
         * @return FrameBody 프레임 바이너리 데이터
         */
        FrameBody RetrieveFrame(int frameNumber) const;

        /**
         * GetQueueSize
         * - 현재 큐에 저장된 데이터 개수를 반환
         * @return 큐의 크기
         */
        size_t GetQueueSize() const;

        /**
         * GetMaxQueueSize
         * - 큐의 최대 크기를 반환
         * @return 최대 큐 크기
         */
        size_t GetMaxQueueSize() const;

        /**
         * SetMaxQueueSize
         * - 큐의 최대 크기를 설정
         * @param maxQueueSize 설정할 최대 크기
         */
        void SetMaxQueueSize(size_t maxQueueSize);
    };
}

#endif // STORAGE_H
