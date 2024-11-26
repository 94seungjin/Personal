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
			{"StartTimestamp", StartTimestamp},
			{"EndTimestamp", EndTimestamp},
			{"FrameStartPointer", FrameStartPointer},
			{"FrameEndPointer", FrameEndPointer},
			{"Width", Width},
			{"Height", Height}
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
		try
		{
			std::filesystem::create_directories(mVideoHeaderDir);
			std::filesystem::create_directories(mFrameHeaderDir);
			std::filesystem::create_directories(mFrameBodyDir);
			std::cout << "Storage: Directories initialized or already exist.\n";
		}
		catch (const std::exception& e)
		{
			std::cerr << "Storage: Failed to initialize directories. Error: " << e.what() << "\n";
		}
	}
	
	// 비디오헤더 저장
	bool Storage::SaveVideoHeader(const VideoHeader& videoHeader)
	{
		// 파일명 생성: 시작 프레임 번호와 마지막 프레임 번호 포함
		std::string fileName = "video_header_" +
								std::to_string(videoHeader.FrameStartPointer) + "_" +
								std::to_string(videoHeader.FrameEndPointer) + ".json";
		std::string filePath = mVideoHeaderDir + "/" + fileName;

		// 파일 열기
		std::ofstream outFile(filePath);
		if (!outFile.is_open())
		{
			std::cerr << "Storage: Failed to open file for VideoHeader: " << filePath << "\n";
			return false;
		}

		// JSON 데이터 저장
		outFile << videoHeader.ToJson() << "\n";
		mVideoHeaders.push_back(videoHeader);

		std::cout << "Storage: VideoHeader saved to " << filePath << "\n";
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
		ClearOldData();
		uint32_t frameNumber = mFrameHeaders.back().FrameNumber;
		std::string filePath = mFrameBodyDir + "/frame_body_" + std::to_string(frameNumber) + ".bin";
		std::ofstream outFile(filePath, std::ios::binary);
		if (!outFile.is_open())
		{
			std::cerr << "Storage: Failed to open file for FrameBody: " << filePath << "\n";
			return false;
		}
		outFile.write(reinterpret_cast<const char*>(frameBody.RawData.data()), frameBody.RawData.size());
		mFrameBodies.push_back(frameBody);
		std::cout << "Storage: FrameBody saved to " << filePath << "\n";
		return true;
	}

		
	// 비디오헤더 검색
	bool Storage::SearchVideoHeader(uint64_t timestamp, VideoHeader& videoHeader)
	{
		for (const auto& header : mVideoHeaders)
		{
			if (timestamp >= header.StartTimestamp && timestamp <= header.EndTimestamp)
			{
				videoHeader = header;
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
		frameBody.RawData.assign(
			std::istreambuf_iterator<char>(inFile),
			std::istreambuf_iterator<char>());
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
		if (mFrameHeaders.size() > mMaxQueueSize)
		{
			uint32_t frameNumber = mFrameHeaders.front().FrameNumber;
			
			// FrameHeader 삭제
			std::string headerFilePath = mFrameHeaderDir + "/frame_header_" + std::to_string(frameNumber) + ".json";
			if (std::filesystem::remove(headerFilePath))
			{
				std::cout << "Storage: Deleted old FrameHeader file: " << headerFilePath << "\n";
			}
			
			// FrameBody 삭제
			std::string bodyFilePath = mFrameBodyDir + "/frame_body_" + std::to_string(frameNumber) + ".bin";
			if (std::filesystem::remove(bodyFilePath))
			{
				std::cout << "Storage: Deleted old FrameBody file: " << bodyFilePath << "\n";
			}
			
			mFrameHeaders.pop_front();
			mFrameBodies.pop_front();
		}
		
		// VideoHeader 삭제(참조하는 모든 프레임 데이터가 삭제될 경우에)
		while (!mVideoHeaders.empty())
		{
			const VideoHeader& videoHeader = mVideoHeaders.front();
			
			// VideoHeader가 참조하는 모든 프레임 데이터 삭제되었는지 확인
			bool isAllFramesDeleted = true;
			for (size_t frameNum = videoHeader.FrameStartPointer; frameNum <= videoHeader.FrameEndPointer; ++frameNum)
			{
				std::string headerFilePath = mFrameHeaderDir + "/frame_header_" + std::to_string(frameNum) + ".json";
				std::string bodyFilePath = mFrameBodyDir + "/frame_body_" + std::to_string(frameNum) + ".bin";
				
				if (std::filesystem::exists(headerFilePath) || std::filesystem::exists(bodyFilePath))
				{
					isAllFramesDeleted = false;
					break;
				}
			}
			
			// 모든 참조 데이터가 삭제된 경우 VideoHeader 삭제
			if (isAllFramesDeleted)
			{
				std::string videoHeaderFilePath = mVideoHeaderDir + "/video_header.json";
				std::ifstream inFile(videoHeaderFilePath);
				std::ofstream outFile("temp_video_header.json");
				
				std::string line;
				while(std::getline(inFile, line))
				{
					nlohmann::json json = nlohmann::json::parse(line);
					
					if (json["StartTimestamp"] == videoHeader.StartTimestamp &&
                    json["EndTimestamp"] == videoHeader.EndTimestamp)
					{
						std::cout << "Storage: Deleted VideoHeader from file. \n";
						continue; // Skip writing this VideoHeader
					}
					
					outFile << line << "\n";
				}
				
				inFile.close();
				outFile.close();
				
				std::filesystem::remove("temp_video_header.json");
				std::filesystem::remove(videoHeaderFilePath);
				
				mVideoHeaders.pop_front();
			}
			else
			{
				break;	// 가장 오래된 VideoHeader의 데이터가 아직 남아있는 경우
			}
		}
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
		return status.dump(4);
	}
}	// namespace video
	
	
