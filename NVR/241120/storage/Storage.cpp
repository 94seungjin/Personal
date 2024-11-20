#include "Storage.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// 생성자: 디렉토리 초기화
Storage::Storage(const std::string& metadataDirectory, const std::string& rawdataDirectory)
    : metadataDir(metadataDirectory), rawdataDir(rawdataDirectory) {
    // 디렉토리가 없으면 생성
    if (!fs::exists(metadataDir))
	{
        fs::create_directories(metadataDir);
    }
    if (!fs::exists(rawdataDir))
	{
        fs::create_directories(rawdataDir);
    }
}

// 파일 이름 생성
std::string Storage::generateFilename(const std::string& dir, const std::string& timestamp, const std::string& extension) const 
{
    return dir + "/" + timestamp + extension;
}

// 메타데이터 저장
bool Storage::saveMetadata(const Metadata& metadata)
{
    // 파일 이름 생성 (예: /metadata/yyyymmdd.bin)
    std::string filename = generateFilename(metadataDir, metadata.timestamp, ".bin");

    // 파일 열기 (바이너리 모드)
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
	{
        std::cerr << "Failed to open file for metadata: " << filename << std::endl;
        return false;
    }

    // 바이너리 형식으로 데이터 기록
    size_t timestampSize = metadata.timestamp.size();
    size_t cameraIdSize = metadata.cameraId.size();
    size_t infoSize = metadata.info.size();

    // 크기와 데이터를 순서대로 기록
    file.write(reinterpret_cast<const char*>(&timestampSize), sizeof(timestampSize));
    file.write(metadata.timestamp.data(), timestampSize);

    file.write(reinterpret_cast<const char*>(&cameraIdSize), sizeof(cameraIdSize));
    file.write(metadata.cameraId.data(), cameraIdSize);

    file.write(reinterpret_cast<const char*>(&infoSize), sizeof(infoSize));
    file.write(metadata.info.data(), infoSize);

    file.close();
    return true;
}
// 원본 데이터 저장
bool Storage::saveRawData(const RawData& rawdata)
{
    std::string filename = generateFilename(rawdataDir, rawdata.timestamp, ".bin");

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
	{
        std::cerr << "Failed to open file for raw data: " << filename << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(rawdata.data.data()), rawdata.data.size());
    file.close();
    return true;
}

// 메타데이터 읽기
Metadata Storage::loadMetadata(const std::string& timestamp) const
{
    // 파일 이름 생성 (예: /metadata/yyyymmdd.bin)
    std::string filename = generateFilename(metadataDir, timestamp, ".bin");

    // 파일 열기 (바이너리 모드)
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
	{
        throw std::runtime_error("Failed to open metadata file: " + filename);
    }

    Metadata metadata;
    size_t timestampSize, cameraIdSize, infoSize;

    // 바이너리 데이터를 순서대로 읽기
    file.read(reinterpret_cast<char*>(&timestampSize), sizeof(timestampSize));
    metadata.timestamp.resize(timestampSize);
    file.read(metadata.timestamp.data(), timestampSize);

    file.read(reinterpret_cast<char*>(&cameraIdSize), sizeof(cameraIdSize));
    metadata.cameraId.resize(cameraIdSize);
    file.read(metadata.cameraId.data(), cameraIdSize);

    file.read(reinterpret_cast<char*>(&infoSize), sizeof(infoSize));
    metadata.info.resize(infoSize);
    file.read(metadata.info.data(), infoSize);

    file.close();
    return metadata;
}

RawData Storage::loadRawData(const std::string& timestamp) const
{
	std::string filename = generateFilename(rawdataDir, timestamp, ".bin");

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open raw data file: " + filename);
	}
	RawData rawdata;
	rawdata.timestamp = timestamp;

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	rawdata.data.resize(size);
	file.read(reinterpret_cast<char*>(rawdata.data.data()), size);

	file.close();
	return rawdata;
}


