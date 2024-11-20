#include "Storage.hpp"
#include <iostream>

int main()
{
    const size_t queueSize = 5;
    const std::string metadataDir = "metadata";
    const std::string rawdataDir = "rawdata";
    const std::string videoPath = "cctv.mp4";

    Storage storage(queueSize, metadataDir, rawdataDir);

    if (storage.extractMetadataAndRawData(videoPath)) {
        std::cout << "Metadata and RawData extraction successful!" << std::endl;

        Metadata meta = storage.loadMetadata("Video_Start");
        std::cout << "Metadata Loaded: " << meta.frameWidth << "x" << meta.frameHeight
                  << ", Total Frames: " << meta.totalFrames << std::endl;

        RawData raw = storage.loadRawData("frame_0");
        std::cout << "First frame size: " << raw.data.size() << " bytes" << std::endl;
    } else {
        std::cerr << "Extraction failed." << std::endl;
    }

    return 0;
}

