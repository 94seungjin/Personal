#include <fstream>
#include <string>

void createMetadataFile(const std::string& directory, const std::string& timestamp) {
    std::string filename = directory + "/" + timestamp + ".bin";
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Example metadata
    std::string cameraId = "CAM01";
    std::string info = "Object detected: Person";

    size_t timestampSize = timestamp.size();
    size_t cameraIdSize = cameraId.size();
    size_t infoSize = info.size();

    // Write metadata to binary file
    file.write(reinterpret_cast<const char*>(&timestampSize), sizeof(timestampSize));
    file.write(timestamp.data(), timestampSize);

    file.write(reinterpret_cast<const char*>(&cameraIdSize), sizeof(cameraIdSize));
    file.write(cameraId.data(), cameraIdSize);

    file.write(reinterpret_cast<const char*>(&infoSize), sizeof(infoSize));
    file.write(info.data(), infoSize);

    file.close();
}

int main() {
    createMetadataFile("./metadata", "20231120_123000");
    return 0;
}

