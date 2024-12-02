#ifndef VIDEOHEADER_H
#define VIDEOHEADER_H

#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>
#include "Serialize.h"

namespace video {

struct VideoHeader {
    uint32_t totalFrames;
    uint16_t width;
    uint16_t height;
    uint32_t frameRate;
    char creationTime[19];
    uint64_t startPoint;
    uint64_t endPoint;

    // 바이너리 직렬화/역직렬화
    void Serialize(std::vector<uint8_t>& buffer) const;
    void Deserialize(const std::vector<uint8_t>& buffer);

    // JSON 직렬화/역직렬화
    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& json);
};

} // namespace video

#endif // VIDEOHEADER_H
