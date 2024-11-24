#include "Storage.h"
#include <iostream>

namespace video
{
    // 생성자: 큐 최대 크기 설정 및 디렉토리 초기화
    Storage::Storage(size_t maxQueueSize)
        : mMaxQueueSize(maxQueueSize)
    {
        initializeDirectories();
    }

    // 필요한 디렉토리 초기화
    void Storage::initializeDirectories()
    {
        try
        {
            fs::create_directories(mHeaderDir); // 헤더 디렉토리 생성
            fs::create_directories(mDataDir);   // 데이터 디렉토리 생성
			std::cout << "Directories initialized successfully." << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error creating directories: " << e.what() << std::endl;
            throw;
        }
    }

    // 헤더 파일 경로 생성
    std::string Storage::getHeaderFilePath(int frameNumber) const
    {
        return mHeaderDir + "/FrameHeader_" + std::to_string(frameNumber) + ".json";
    }

    // 데이터 파일 경로 생성
    std::string Storage::getDataFilePath(int frameNumber) const
    {
        return mDataDir + "/FrameData_" + std::to_string(frameNumber) + ".bin";
    }

    // 큐가 초과될 때 가장 오래된 데이터를 삭제
    void Storage::removeOldestData()
    {
        if (!mHeaderQueue.empty() && !mDataQueue.empty())
        {
            int frameNumber = mHeaderQueue.front().FrameNumber; // 가장 오래된 프레임 번호
            std::string headerFilePath = getHeaderFilePath(frameNumber); // 헤더 경로
            std::string dataFilePath = mDataQueue.front(); // 데이터 경로

            // 파일 삭제
            if (fs::exists(headerFilePath)) fs::remove(headerFilePath);
            if (fs::exists(dataFilePath)) fs::remove(dataFilePath);

            // 큐에서 제거
            mHeaderQueue.pop_front();
            mDataQueue.pop_front();
        }
    }

    // 프레임 데이터 저장
    void Storage::StoreFrame(const FrameHeader& header, const FrameBody& body)
    {
        std::string headerFilePath = getHeaderFilePath(header.FrameNumber); // 헤더 경로 생성
        std::string dataFilePath = getDataFilePath(header.FrameNumber);     // 데이터 경로 생성

        try
        {
            // 헤더 저장
            json headerJson = {
                {"timestamp", header.Timestamp},
                {"frameNumber", header.FrameNumber},
                {"width", header.Width},
                {"height", header.Height},
                {"compression", header.Compression}
            };

            std::ofstream headerFile(headerFilePath); // 헤더 파일 열기
            if (!headerFile)
            {
                throw std::ios_base::failure("Failed to open header file: " + headerFilePath);
            }
            headerFile << headerJson.dump(4); // JSON 데이터 저장

            // 데이터 저장
            std::ofstream dataFile(dataFilePath, std::ios::binary); // 데이터 파일 열기
            if (!dataFile)
            {
                throw std::ios_base::failure("Failed to open data file: " + dataFilePath);
            }
            dataFile.write(reinterpret_cast<const char*>(body.RawData.data()), body.RawData.size());

            // 큐에 추가
            mHeaderQueue.push_back(header);
            mDataQueue.push_back(dataFilePath);

            // 큐 크기 관리
            if (mHeaderQueue.size() > mMaxQueueSize)
            {
                removeOldestData();
            }
        }
        catch (const std::exception& e)
        {
            // 오류 발생 시 롤백
            if (fs::exists(headerFilePath)) fs::remove(headerFilePath);
            if (fs::exists(dataFilePath)) fs::remove(dataFilePath);

            std::cerr << "Error storing frame: " << e.what() << std::endl;
        }
    }

    // 특정 프레임 데이터 검색
    FrameBody Storage::RetrieveFrame(int frameNumber) const
    {
        std::string dataFilePath = getDataFilePath(frameNumber);

        if (!fs::exists(dataFilePath))
        {
            throw std::runtime_error("Frame data file does not exist: " + dataFilePath);
        }

        std::ifstream dataFile(dataFilePath, std::ios::binary);
        if (!dataFile)
        {
            throw std::runtime_error("Failed to open frame data file: " + dataFilePath);
        }

        dataFile.seekg(0, std::ios::end);
        size_t fileSize = dataFile.tellg();
        dataFile.seekg(0, std::ios::beg);

        FrameBody body;
        body.RawData.resize(fileSize);
        dataFile.read(reinterpret_cast<char*>(body.RawData.data()), fileSize);

        return body;
    }

    // 현재 큐 크기 반환
    size_t Storage::GetQueueSize() const
    {
        return mHeaderQueue.size();
    }

    // 최대 큐 크기 반환
    size_t Storage::GetMaxQueueSize() const
    {
        return mMaxQueueSize;
    }

    // 최대 큐 크기 설정
    void Storage::SetMaxQueueSize(size_t maxQueueSize)
    {
        mMaxQueueSize = maxQueueSize;
    }
}
