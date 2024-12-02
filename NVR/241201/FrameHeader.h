#ifndef FRAMEHEADER_H
#define FRAMEHEADER_H

#include <cstdint>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "Serialize.h"

namespace video {

/**
 * @brief 개별 비디오 프레임의 메타데이터를 저장하는 구조체
 */
struct FrameHeader
{
    uint32_t frameId;
    uint32_t bodySize;
    char timestamp[19]; // "YYYYMMDD_HHMMSS.sss"

    // Serialize to binary
    void Serialize(std::vector<uint8_t>& buffer) const;
    void Deserialize(const std::vector<uint8_t>& buffer);

    // Serialize to JSON
    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& json);
};

} // namespace video

#endif // FRAMEHEADER_H
