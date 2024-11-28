#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <iostream>

namespace video
{

/**
 * @brief 비디오 파일의 메타데이터를 저장하는 구조체
 */
struct VideoHeader
{
    uint32_t totalFrames;        ///< 비디오의 총 프레임 수
    uint16_t width;              ///< 비디오 가로 해상도 (픽셀 단위)
    uint16_t height;             ///< 비디오 세로 해상도 (픽셀 단위)
    uint32_t frameRate;          ///< 비디오 초당 프레임 수 (FPS)
    char creationTime[19];       ///< 비디오 생성 시간 ("YYYY-MM-DD HH:MM:SS" 형식)
    uint8_t padding1[1];         ///< 데이터 정렬을 위한 패딩
    uint64_t startPoint;         ///< 파일에서 비디오 데이터의 시작 위치
    uint64_t endPoint;           ///< 파일에서 비디오 데이터의 끝 위치

    /**
     * @brief VideoHeader 데이터를 직렬화하여 버퍼에 저장
     * @param buffer 직렬화된 데이터를 저장할 버퍼
     */
    void Serialize(std::vector<uint8_t>& buffer) const
    {
        buffer.resize(sizeof(VideoHeader));
        std::memcpy(buffer.data(), this, sizeof(VideoHeader));
    }

    /**
     * @brief 버퍼에서 VideoHeader 데이터를 역직렬화하여 복원
     * @param buffer 직렬화된 데이터를 담고 있는 버퍼
     * @throw std::runtime_error 버퍼 크기가 충분하지 않을 경우 예외 발생
     */
    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        if (buffer.size() < sizeof(VideoHeader))
        {
            throw std::runtime_error("Buffer size is too small to deserialize VideoHeader");
        }
        std::memcpy(this, buffer.data(), sizeof(VideoHeader));
    }
};

/**
 * @brief 개별 비디오 프레임의 메타데이터를 저장하는 구조체
 */
struct FrameHeader
{
    uint32_t frameId;            ///< 프레임 고유 ID
    uint32_t bodySize;           ///< 프레임 본문의 크기 (바이트 단위)
    char timestamp[19];          ///< 프레임의 타임스탬프 ("YYYY-MM-DD HH:MM:SS" 형식)

    /**
     * @brief FrameHeader 데이터를 직렬화하여 버퍼에 저장
     * @param buffer 직렬화된 데이터를 저장할 버퍼
     */
    void Serialize(std::vector<uint8_t>& buffer) const
    {
        buffer.resize(sizeof(FrameHeader));
        std::memcpy(buffer.data(), this, sizeof(FrameHeader));
    }

    /**
     * @brief 버퍼에서 FrameHeader 데이터를 역직렬화하여 복원
     * @param buffer 직렬화된 데이터를 담고 있는 버퍼
     * @throw std::runtime_error 버퍼 크기가 충분하지 않을 경우 예외 발생
     */
    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        if (buffer.size() < sizeof(FrameHeader))
        {
            throw std::runtime_error("Buffer size is too small to deserialize FrameHeader");
        }
        std::memcpy(this, buffer.data(), sizeof(FrameHeader));
    }
};

/**
 * @brief 프레임 본문(데이터)을 저장하는 구조체
 */
struct FrameBody
{
    std::vector<uint8_t> data;   ///< 프레임의 실제 데이터 (바이너리 형식)

    /**
     * @brief FrameBody 데이터를 직렬화하여 버퍼에 저장
     * @param buffer 직렬화된 데이터를 저장할 버퍼
     */
    void Serialize(std::vector<uint8_t>& buffer) const
    {
        buffer = data;
    }

    /**
     * @brief 버퍼에서 FrameBody 데이터를 역직렬화하여 복원
     * @param buffer 직렬화된 데이터를 담고 있는 버퍼
     */
    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        data = buffer;
    }
};

/**
 * @brief 프레임을 메모리에서 효율적으로 관리하기 위한 순환 큐
 */
class FrameQueue
{
public:
    /**
     * @brief FrameQueue 생성자
     * @param capacity 큐의 최대 용량 (프레임 수)
     */
    explicit FrameQueue(size_t capacity)
        : mCapacity(capacity), mHead(0), mTail(0), mSize(0), mQueue(capacity) {}

    /**
     * @brief 큐가 비어 있는지 확인
     * @return 큐가 비어 있으면 true, 아니면 false
     */
    bool IsEmpty() const { return mSize == 0; }

    /**
     * @brief 큐가 가득 찼는지 확인
     * @return 큐가 가득 차 있으면 true, 아니면 false
     */
    bool IsFull() const { return mSize == mCapacity; }

    /**
     * @brief 큐에 프레임 추가
     * @param header 프레임 메타데이터
     * @param body 프레임 본문 데이터
     */
    void Push(const FrameHeader& header, const FrameBody& body)
    {
        mQueue[mTail] = {header, body};
        mTail = (mTail + 1) % mCapacity;
        if (IsFull())
        {
            mHead = (mHead + 1) % mCapacity;
        }
        else
        {
            mSize++;
        }
    }

    /**
     * @brief 큐에서 가장 오래된 프레임 제거 및 반환
     * @return 제거된 프레임의 헤더와 본문 데이터
     * @throw std::runtime_error 큐가 비어 있는 경우 예외 발생
     */
    std::pair<FrameHeader, FrameBody> Pop()
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Queue is empty");
        }
        auto frame = mQueue[mHead];
        mHead = (mHead + 1) % mCapacity;
        mSize--;
        return frame;
    }

private:
    size_t mCapacity; ///< 큐의 최대 용량
    size_t mHead;     ///< 가장 오래된 프레임의 인덱스
    size_t mTail;     ///< 새로 추가될 프레임의 인덱스
    size_t mSize;     ///< 큐에 저장된 프레임 수
    std::vector<std::pair<FrameHeader, FrameBody>> mQueue; ///< 프레임 저장 공간
};

/**
 * @brief 비디오 데이터의 저장 및 관리를 담당하는 클래스
 */
class Storage
{
public:
    /**
     * @brief Storage 생성자
     * @param filePath 비디오 파일 경로
     * @param maxFrames 큐의 최대 프레임 수
     */
    Storage(const std::string& filePath, size_t maxFrames);
    ~Storage();

    /**
     * @brief 비디오 파일을 초기화하고 메타데이터를 설정
     * @param videoHeader 비디오 메타데이터
     */
    void InitializeVideo(const VideoHeader& videoHeader);

    /**
     * @brief 비디오 파일에 프레임을 저장
     * @param header 프레임 메타데이터
     * @param body 프레임 본문 데이터
     */
    void SaveFrame(const FrameHeader& header, const FrameBody& body);

    /**
     * @brief 큐에서 다음 프레임을 가져옴
     * @param header 가져온 프레임의 메타데이터
     * @param body 가져온 프레임의 본문 데이터
     * @return 프레임이 존재하면 true, 없으면 false
     */
    bool GetNextFrame(FrameHeader& header, FrameBody& body);

    /**
     * @brief 비디오 파일을 최종화하고 메타데이터를 업데이트
     */
    void FinalizeVideo();

private:
    std::string mFilePath;         ///< 비디오 파일 경로
    std::ofstream mFileStream;     ///< 파일 출력 스트림
    std::ifstream mFileInputStream; ///< 파일 입력 스트림
    FrameQueue mFrameQueue;        ///< 프레임 큐

    VideoHeader mVideoHeader;      ///< 비디오 메타데이터
    uint32_t mCurrentFrameId;      ///< 현재 처리 중인 프레임 ID

    /**
     * @brief 프레임 헤더와 본문을 비디오 파일에 저장
     * @param header 프레임 메타데이터
     * @param body 프레임 본문 데이터
     */
    void writeFrameToFile(const FrameHeader& header, const FrameBody& body);
};

} // namespace video

#endif // STORAGE_H
