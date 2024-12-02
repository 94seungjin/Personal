#include "FrameHeader.h"
#include <cstring>

namespace video {

void FrameHeader::Serialize(std::vector<uint8_t>& buffer) const
{
    utils::Serialize(*this, buffer);
}

void FrameHeader::Deserialize(const std::vector<uint8_t>& buffer)
{
    utils::Deserialize(buffer, *this);
}

nlohmann::json FrameHeader::ToJson() const
{
    return {
        {"frame_id", frameId},
        {"body_size", bodySize},
        {"timestamp", std::string(timestamp)}
    };
}

void FrameHeader::FromJson(const nlohmann::json& json)
{
    frameId = json.at("frame_id").get<uint32_t>();
    bodySize = json.at("body_size").get<uint32_t>();

    std::string time = json.at("timestamp").get<std::string>();
    std::memcpy(timestamp, time.c_str(), sizeof(timestamp));
}

} // namespace video
