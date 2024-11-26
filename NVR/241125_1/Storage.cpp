#include "Storage.h"

#include <iostream>
#include <filesystem>

namespace video
{
	std::string FrameHeader::ToJson() const
	{
		nlohmann::json jsonObject =
		{
			{"FrameTimestamp", FrameTimestamp},
			{"FrameNumber", FrameNumber},
			{"Width", Width},
			{"Height", Height},
			{"Compression", Compression}
		};
		return jsonObject.dump();
	}
	
	std::string VideoHeader::ToJson() const
	{
		nlohmann::json jsonObject =
		{
			{"FrameEndPointer", FrameEndPointer}
		};
		return jsonObject.dump();
	}
	
	// 생성자
	Storage::Storage(const std::string& baseDir, size_t maxQueueSize)
		: mDir(baseDir), mMaxQueueSize(maxQueueSize)
		{
			mVideoHeaderDir = mDir + "/video_headers";
			mFrameHeaderDir = mDir + "/frame_headers";
			mFrameBodyDir = mDir + "/frame_bodies";
			initDir();
		}
		
	// 소멸자
	Storage::~Storage()
	{
	}
	
	// 디렉토리 초기화
	void Storage::initDir()
	{
		if (!std::filesystem::create_directories(mVideoHeaderDir))
		{
			std::cout << "Storage: VideoHeader directory already exists or created successfully.\n";
		}
		if (!std::filesystem::create_directories(mFrameHeaderDir))
		{
			std::cout << "Storage: FrameHeader directory already exists or created successfully.\n";
		}
		if (!std::filesystem::create_directories(mFrameBodyDir))
		{
			std::cout << "Storage: FrameBody directory already exists or created successfully.\n";
		}
	}
	
	// 비디오헤더 저장
	bool Storage::SaveVideoHeader(const VideoHeader& videoHeader)
	{
		std::ofstream outFile(mVideoHeaderDir + "/video_header.json", std::ios::app);
		if (!outFile.is_open())
		{
			return false;
		}
		outFile << videoHeader.ToJson() << std::endl;
		return true;
	}
	
	// 프레임헤더 저장
	bool Storage::SaveFrameHeader(const FrameHeader& frameHeader)
	{
		ClearOldData(); // 데이터 저장 전 오래된 데이터 삭제
		std::string filePath = mFrameHeaderDir + "/frame_header_" + std::to_string(frameHeader.FrameNumber) + ".json";
		std::ofstream outFile(filePath);
		if (!outFile.is_open())
		{
			std::cerr << "Storage: Failed to open file for FrameHeader: " << filePath << "\n";
			return false;
		}
		outFile << frameHeader.ToJson();
		mFrameHeaders.push_back(frameHeader); // 큐에 데이터 추가
		std::cout << "Storage: FrameHeader saved to " << filePath << "\n";
    return true;
	}

	// 프레임바디 저장
	bool Storage::SaveFrameBody(const FrameBody& frameBody)
	{
		ClearOldData(); // 오래된 데이터 삭제

		// FrameHeader의 FrameNumber 기반으로 파일 이름 생성
		uint32_t frameNumber = mFrameHeaders.back().FrameNumber; // 가장 최근 저장된 FrameHeader의 FrameNumber 사용
		std::string filePath = mFrameBodyDir + "/frame_body_" + std::to_string(frameNumber) + ".bin";

		// FrameBody 데이터 추가
		mFrameBodies.push_back(frameBody);

		// 파일 저장
		std::ofstream outFile(filePath, std::ios::binary);
		if (!outFile.is_open())
		{
			std::cerr << "Storage: Failed to open file for FrameBody: " << filePath << "\n";
			return false;
		}
		outFile.write(reinterpret_cast<const char*>(frameBody.RawData.data()), frameBody.RawData.size());
		std::cout << "Storage: FrameBody saved to " << filePath << "\n";
		return true;
	}

		
	// 비디오헤더 검색
	bool Storage::SearchVideoHeader(size_t frameEndPointer, VideoHeader& videoHeader)
	{
		std::ifstream inFile(mVideoHeaderDir + "/video_header.json");
		if (!inFile.is_open())
		{
			return false;
		}
		
		std::string line;
		while (std::getline(inFile, line))
		{
			auto json = nlohmann::json::parse(line);
			if (json["FrameEndPointer"] == frameEndPointer)
			{
				videoHeader.FrameEndPointer = frameEndPointer;
				return true;
			}
		}
		return false;
	}
	
	// 프레임헤더 검색
	bool Storage::SearchFrameHeader(uint64_t frameTimestamp, FrameHeader& frameHeader)
	{
		for (const auto& entry : std::filesystem::directory_iterator(mFrameHeaderDir))
		{
			std::ifstream inFile(entry.path());
			if (!inFile.is_open())
			{
				continue;
			}
			
			auto json = nlohmann::json::parse(inFile);
			if (json["FrameTimestamp"] == frameTimestamp)
			{
				frameHeader.FrameTimestamp = json["FrameTimestamp"];
				frameHeader.FrameNumber = json["FrameNumber"];
				frameHeader.Width = json["Width"];
				frameHeader.Height = json["Height"];
				frameHeader.Compression = json["Compression"];
				return true;
			}
		}
		return false;
	}
	
	// 프레임바디 검색
	bool Storage::SearchFrameBody(uint32_t frameNumber, FrameBody& frameBody)
	{
		std::string filePath = mFrameBodyDir + "/frame_body_" + std::to_string(frameNumber) + ".bin";
		std::ifstream inFile(filePath, std::ios::binary);
		if (!inFile.is_open())
		{
			return false;
		}
		
		frameBody.RawData = std::vector<uint8_t>
		(
			std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>()
		);
		return true;
	}
	
	// 비디오헤더 송신
	bool Storage::SendVideoHeader(const VideoHeader& videoHeader)
	{
		std::cout << "Sending VideoHeader: " << videoHeader.ToJson() << std::endl;
		return true;	// 전송 성공 시 true 반환
	}
	
	// 프레임헤더 송신
	bool Storage::SendFrameHeader(const FrameHeader& frameHeader)
	{
		std::cout << "Sending FrameHeader: " << frameHeader.ToJson() << std::endl;
		return true;
	}
	
	// 프레임바디 송신
	bool Storage::SendFrameBody(const FrameBody& frameBody)
	{
		std::cout << "Sending FrameBody: [binary data of size " << frameBody.RawData.size() << "]" << std::endl;
		return true;
	}
	
	// 오래된 데이터 삭제
	void Storage::ClearOldData()
	{
		// FrameHeader와 FrameBody를 FrameHeader의 FrameNumber 기준으로 삭제
		if (mFrameHeaders.size() > mMaxQueueSize)
		{
			uint32_t frameNumber = mFrameHeaders.front().FrameNumber;
			// FrameHeader 삭제
			std::string headerFilePath = mFrameHeaderDir + "/frame_header_" + std::to_string(frameNumber) + ".json";
			if (std::filesystem::remove(headerFilePath))
			{
				std::cout << "Storage: Deleted old FrameHeader file: " << headerFilePath << "\n";
			}
			mFrameHeaders.pop_front();

			// FrameBody 삭제
			std::string bodyFilePath = mFrameBodyDir + "/frame_body_" + std::to_string(frameNumber) + ".bin";
			if (std::filesystem::remove(bodyFilePath))
			{
				std::cout << "Storage: Deleted old FrameBody file: " << bodyFilePath << "\n";
			}
			mFrameBodies.pop_front();
		}

		/*// VideoHeader 삭제 (별도의 기준이 있을 경우 추가 처리)
		if (mVideoHeaders.size() > mMaxQueueSize)
		{
			std::string filePath = mVideoHeaderDir + "/video_header_" + std::to_string(mVideoHeaders.front().FrameEndPointer) + ".json";
			if (std::filesystem::remove(filePath))
			{
				std::cout << "Storage: Deleted old VideoHeader file: " << filePath << "\n";
			}
			mVideoHeaders.pop_front();
		}*/
	}

	
	std::string Storage::GetStatus() const
	{
		nlohmann::json status =
		{
			{"BaseDir", mDir},
			{"VideoHeaderDir", mVideoHeaderDir},
			{"FrameHeaderDir", mFrameHeaderDir},
			{"FrameBodyDir", mFrameBodyDir},
			{"MaxQueueSize", mMaxQueueSize},
			{"CurrentVideoHeaderQueueSize", mVideoHeaders.size()},
			{"CurrentFrameHeaderQueueSize", mFrameHeaders.size()},
			{"CurrentFrameBodyQueueSize", mFrameBodies.size()}
		};
		return status.dump(4); // JSON 문자열 반환 (들여쓰기 포함)
	}
}	// namespace video
	
	
