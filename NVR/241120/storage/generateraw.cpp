#include <fstream>
#include <vector>
#include <string>

void createRawDataFile(const std::string& directory, const std::string& timestamp) {
    std::string filename = directory + "/" + timestamp + ".bin";

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Example raw data (e.g., encoded video frames)
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};

    // Write raw data to binary file
    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    file.close();
}

int main() {
    createRawDataFile("./rawdata", "20231120_123000");
    return 0;
}

