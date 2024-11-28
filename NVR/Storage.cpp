#include "Storage.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace video {

/**
 * @brief Storage 생성자
 * @param filePath 저장할 비디오 파일 경로
 * @param maxFrames 메모리에 유지할 최대 프레임 수
 * 
 * - 파일 경로와 큐의 용량을 초기화합니다.
 * - mCurrentFrameId는 0으로 초기화되어 첫 프레임부터 시작하도록 설정됩니다.
 */
Storage::Storage(const std::string& filePath, size_t maxFrames)
    : mFilePath(filePath), mFrameQueue(maxFrames), mCurrentFrameId(0) {}

/**
 * @brief Storage 소멸자
 * 
 * - 열려 있는 파일 스트림을 닫아 자원을 해제합니다.
 */
Storage::~Storage()
{
    if (mFileStream.is_open())
    {
        mFileStream.close();
    }
    if (mFileInputStream.is_open())
    {
        mFileInputStream.close();
    }
}

/**
 * @brief 비디오 파일 초기화
 * @param videoHeader 비디오 메타데이터 (VideoHeader)
 * 
 * - mVideoHeader를 주어진 값으로 초기화합니다.
 * - 새 파일을 생성하여 VideoHeader를 기록합니다.
 * - 파일은 기존 파일을 덮어쓰며, 초기화 실패 시 예외를 던집니다.
 */
void Storage::InitializeVideo(const VideoHeader& videoHeader)
{
    mVideoHeader = videoHeader;

    // 비디오 파일 생성 및 쓰기 모드로 열기
    mFileStream.open(mFilePath, std::ios::binary | std::ios::trunc);
    if (!mFileStream)
    {
        throw std::runtime_error("Failed to open file for writing: " + mFilePath);
    }

    // VideoHeader 데이터를 직렬화하여 파일에 저장
    std::vector<uint8_t> headerBuffer;
    mVideoHeader.Serialize(headerBuffer);

    mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
    if (!mFileStream)
    {
        throw std::runtime_error("Failed to write VideoHeader to file.");
    }

    mFileStream.flush(); // 파일에 데이터를 강제로 저장
}

/**
 * @brief 프레임 저장
 * @param header 프레임 메타데이터 (FrameHeader)
 * @param body 프레임 데이터 (FrameBody)
 * 
 * - 큐에 프레임 데이터를 추가합니다.
 * - 파일 스트림을 통해 프레임 데이터를 기록합니다.
 * - 프레임 수를 증가시키고, VideoHeader의 totalFrames를 업데이트합니다.
 */
void Storage::SaveFrame(const FrameHeader& header, const FrameBody& body)
{
    // 파일 스트림이 열려 있지 않거나 오류가 발생하면 다시 열기
    if (!mFileStream.is_open() || mFileStream.fail() || mFileStream.bad())
    {
        mFileStream.open(mFilePath, std::ios::binary | std::ios::app);
        if (!mFileStream)
        {
            throw std::runtime_error("Failed to reopen file stream.");
        }
    }

    // 프레임을 메모리 큐에 추가
    mFrameQueue.Push(header, body);

    // 프레임 데이터를 파일에 기록
    writeFrameToFile(header, body);

    // 프레임 ID 증가 및 VideoHeader 업데이트
    mCurrentFrameId++;
    mVideoHeader.totalFrames = mCurrentFrameId;
}

/**
 * @brief 큐에서 다음 프레임 가져오기
 * @param header 반환될 프레임 메타데이터 (FrameHeader)
 * @param body 반환될 프레임 데이터 (FrameBody)
 * @return 큐에 프레임이 있으면 true, 없으면 false
 * 
 * - 큐에서 프레임을 제거하고 반환합니다.
 * - 큐가 비어 있으면 false를 반환하여 더 이상 프레임이 없음을 나타냅니다.
 */
bool Storage::GetNextFrame(FrameHeader& header, FrameBody& body)
{
    if (mFrameQueue.IsEmpty())
    {
        return false; // 큐가 비어 있는 경우 false 반환
    }

    // 큐에서 가장 오래된 프레임 가져오기
    auto frame = mFrameQueue.Pop();
    header = frame.first; // 헤더 추출
    body = frame.second;  // 본문 추출

    return true; // 성공적으로 프레임을 가져옴
}

/**
 * @brief 비디오 파일 최종화
 * 
 * - 현재 파일 포인터를 기록하여 VideoHeader의 endPoint를 업데이트합니다.
 * - 파일의 시작 부분으로 이동하여 최종 VideoHeader를 기록합니다.
 */
void Storage::FinalizeVideo()
{
    if (mFileStream.is_open())
    {
        // 파일의 현재 쓰기 위치를 VideoHeader의 endPoint에 저장
        mVideoHeader.endPoint = mFileStream.tellp();

        // 파일의 시작으로 이동하여 VideoHeader 업데이트
        mFileStream.seekp(0);

        std::vector<uint8_t> headerBuffer;
        mVideoHeader.Serialize(headerBuffer);
        mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
        mFileStream.flush();
        mFileStream.close(); // 파일 스트림 닫기
    }
}

/**
 * @brief 프레임 데이터를 파일에 기록
 * @param header 프레임 메타데이터 (FrameHeader)
 * @param body 프레임 데이터 (FrameBody)
 * 
 * - FrameHeader와 FrameBody 데이터를 각각 직렬화하여 파일에 저장합니다.
 * - 파일 스트림 상태를 확인하여 쓰기 실패 시 예외를 던집니다.
 */
void Storage::writeFrameToFile(const FrameHeader& header, const FrameBody& body)
{
    if (!mFileStream.is_open())
    {
        throw std::runtime_error("File stream is not open for writing.");
    }

    // FrameHeader 데이터를 직렬화하여 파일에 저장
    std::vector<uint8_t> headerBuffer;
    header.Serialize(headerBuffer);
    mFileStream.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
    if (mFileStream.fail())
    {
        throw std::runtime_error("Failed to write FrameHeader to file.");
    }

    // FrameBody 데이터를 직렬화하여 파일에 저장
    std::vector<uint8_t> bodyBuffer;
    body.Serialize(bodyBuffer);
    mFileStream.write(reinterpret_cast<const char*>(bodyBuffer.data()), bodyBuffer.size());
    if (mFileStream.fail())
    {
        throw std::runtime_error("Failed to write FrameBody to file.");
    }

    mFileStream.flush(); // 파일에 데이터를 강제로 저장
}

} // namespace video
