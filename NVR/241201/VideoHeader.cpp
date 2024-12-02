#include "VideoHeader.h"
#include <cstring>

namespace video {

// 바이너리 직렬화
void VideoHeader::Serialize(std::vector<uint8_t>& buffer) const
{
    utils::Serialize(*this, buffer);
}

// 바이너리 역직렬화
void VideoHeader::Deserialize(const std::vector<uint8_t>& buffer)
{
    utils::Deserialize(buffer, *this);
}

// JSON 직렬화
nlohmann::json VideoHeader::ToJson() const
{
    return {
        {"total_frames", totalFrames},
        {"width", width},
        {"height", height},
        {"frame_rate", frameRate},
        {"creation_time", std::string(creationTime)},
        {"start_point", startPoint},
        {"end_point", endPoint}
    };
}

// JSON 역직렬화
void VideoHeader::FromJson(const nlohmann::json& json)
{
    totalFrames = json.at("total_frames").get<uint32_t>();
    width = json.at("width").get<uint16_t>();
    height = json.at("height").get<uint16_t>();
    frameRate = json.at("frame_rate").get<uint32_t>();

    std::string time = json.at("creation_time").get<std::string>();
    std::memcpy(creationTime, time.c_str(), sizeof(creationTime));

    startPoint = json.at("start_point").get<uint64_t>();
    endPoint = json.at("end_point").get<uint64_t>();
}

} // namespace video
