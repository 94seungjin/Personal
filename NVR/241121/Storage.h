#ifndef STORAGE_H
#define STORAGE_H

#include <opencv2/opencv.hpp>
#include <deque>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>

namespace video
{
	using json = nlohmann::json;
	namespace fs = std::filesystem;

	struct VideoHeader
	{
		std::string videoTimestamp;		// 프레임 캡처 시간
		int frameNumber;				// 프레임 번호
		cv::Size resolution;			// 해상도(width x height)
		std::string compression;		// 압축 형식(jpeg)
		std::streampos filePointer;		// 파일 내 위치 포인터

		// JSON 변환 기능 추가
		json toJson() const
		{
			return
			{
				{"videoTimestamp", videoTimestamp},
				{"frameNumber", frameNumber},
				{"resolution", {resolution.width, resolution.height}},
				{"compression", compression}
			};
		}
	};

	struct VideoData
	{
		std::vector<uchar> rawData;		// 원본 데이터 (JPEG 바이트)
	};

	class Storage
	{
		private:
			// Circular Queue
			std::deque<VideoHeader> videoHeaderDeque;
			std::deque<std::string> videoDataDeque;		// 파일 이름만 저장(데이터 파일 경로)

			// 최대 큐 크기
			const size_t maxQueueSize = 100;

			// 디렉토리 경로
			const std::string baseDir = "storage";
			const std::string headerDir = "storage/videoheader";
			const std::string dataDir = "storage/videodata";

			// 내부 함수
			void initializeDir();		// 디렉토리 생성
			void removeOldestData();	// 큐가 가득 찼을 때 가장 오래된 데이터 제거

		public:
			Storage();
			
			void ExtractAndStoreFrames(const std::string& videoPath);	// 프레임 추출 및 저장
	};

}

#endif	// STORAGE_H

