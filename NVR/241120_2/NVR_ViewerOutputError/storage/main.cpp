#include "Storage.hpp"
#include <iostream>

int main() {
    const std::string metadataDir = "metadata";
    const std::string rawdataDir = "rawdata";
    const size_t maxSize = 5;

    Storage storage(maxSize, metadataDir, rawdataDir);

    // 테스트용 메타데이터 및 RawData
    Metadata meta = {"20231120_123000", 1920, 1080, 500};
    RawData raw = {"20231120_123000", {0x00, 0x01, 0x02, 0x03}};

    // 저장 테스트
    storage.saveMetadata(meta);
    storage.saveRawData(raw);

    // 로드 테스트
    try {
        Metadata loadedMeta = storage.loadMetadata("20231120_123000");
        RawData loadedRaw = storage.loadRawData("20231120_123000");

        std::cout << "Loaded Metadata: " << loadedMeta.timestamp << ", "
                  << loadedMeta.frameWidth << "x" << loadedMeta.frameHeight
                  << ", Frames: " << loadedMeta.totalFrames << std::endl;

        std::cout << "Loaded RawData Size: " << loadedRaw.data.size() << " bytes" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

