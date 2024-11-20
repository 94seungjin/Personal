#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>

struct Metadata
{
	std::string timestamp;			// 타임스탬프
	std::string cameraId;			// 카메라 ID
	std::string info;				// 기타 메타데이터 정보
};

struct RawData
{
	std::string timestamp;			// 타임스탬프
	std::vector<uint8_t> data;		// 원본 데이터
};

class Storage
{
	private:
		std::string metadataDir;	// 메타데이터 저장 dir
		std::string rawdataDir;		// 원본 데이터 저장 dir

		std::string generateFilename(const std::string& dir, const std::string& timestamp, const std::string& extension) const;

	public:
		Storage(const std::string& metadataDirectory, const std::string& rawdataDirectory);
		// 데이터 저장(binary)
		bool saveMetadata(const Metadata& metadata);
		bool saveRawData(const RawData& rawdata);

		// 데이터 읽기(binary)
		Metadata loadMetadata(const std::string& timestamp) const;
		RawData loadRawData(const std::string& timestamp) const;
};

#endif	// STORAGE_H
