#include "Storage.h"
#include <iostream>

namespace video
{
	Storage::Storage()
	{
		initializeDir();
	}

	// 디렉토리 초기화
	void Storage::initializeDir()
	{
		try
		{
			fs::create_directories(headerDir);
			fs::create_directories(dataDir);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error creating directories: " << e.what() << std::endl;
			throw;
		}
	}

	// 가장 오래된 데이터 제거
	void Storage::removeOldestData()
	{
		if (!videoHeaderDeque.empty() && !videoDataDeque.empty())
		{
			// 비디오 헤더파일 삭제
			const auto& oldestHeader = videoHeaderDeque.front();
			const std::string headerFile = headerDir + "/cam01_VideoHeader_" + std::to_string(oldestHeader.frameNumber) + ".json";
			fs::remove(headerFile);

			// 비디오 데이터파일 삭제
			const auto& oldestDataFile = videoDataDeque.front();
			fs::remove(oldestDataFile);

			// 큐에서 제거
			videoHeaderDeque.pop_front();
			videoDataDeque.pop_front();
		}
	}

	// 프레임을 추출하고 저장
	void Storage::ExtractAndStoreFrames(const std::string& videoPath)
	{
		cv::VideoCapture cap(videoPath);
		if (!cap.isOpened())
		{
			std::cerr << "Error: Unable to open video file: " << videoPath << std::endl;
			return;
		}

		int frameNumber = 0;
		cv::Mat frame;

		while (cap.read(frame))
		{
			// 프레임 정보 생성
			VideoHeader header;
			header.videoTimestamp = std::to_string(static_cast<int>(cap.get(cv::CAP_PROP_POS_MSEC))) + "ms";
			header.frameNumber = frameNumber++;
			header.resolution = frame.size();
			header.compression = "JPEG";

			// 프레임을 JPEG로 압축
			std::vector<uchar> buffer;
			std::vector<int> compressionParams = {cv::IMWRITE_JPEG_QUALITY, 90};
			cv::imencode(".jpg", frame, buffer, compressionParams);

			// VideoData 생성
			VideoData data;
			data.rawData = buffer;

			// 파일 저장
			const std::string headerFileName = headerDir + "/cam01_VideoHeader_" + std::to_string(header.frameNumber) + ".json";
			const std::string dataFileName = dataDir + "/cam01_VideoData_" + std::to_string(header.frameNumber) + ".jpg";

			// 헤더 파일 저장
			std::ofstream headerFile(headerFileName);
			headerFile << header.toJson().dump(4);		// JSON 형식으로 저장
			headerFile.close();

			// 데이터 파일 저장
			std::ofstream dataFile(dataFileName, std::ios::binary);
			dataFile.write(reinterpret_cast<const char*>(data.rawData.data()), data.rawData.size());
			dataFile.close();

			// 큐에 추가
			videoHeaderDeque.push_back(header);
			videoDataDeque.push_back(dataFileName);

			// 큐가 가득 찼을 경우 오래된 데이터를 제거
			if (videoHeaderDeque.size() > maxQueueSize)
			{
				removeOldestData();
			}
		}
		cap.release();
	}
}		// namespace video


