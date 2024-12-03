#ifndef VIDEOHEADER_H
#define VIDEOHEADER_H

#include <cstdint>
#include <vector>
#include "Serialize.h"

namespace video {

/**
 * @brief 비디오 파일의 메타데이터를 저장하는 구조체
 */
struct VideoHeader
{
    uint32_t totalFrames;
    uint16_t width;
    uint16_t height;
    uint32_t frameRate;
    char creationTime[20];      // "YYYYMMDD_HHMMSS.sss"
    uint8_t padding1[1];
    uint64_t startPoint;
    uint64_t endPoint;

    void Serialize(std::vector<uint8_t>& buffer) const
    {
        utils::Serialize(*this, buffer);
    }

    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        utils::Deserialize(buffer, *this);
    }
};

} // namespace video

#endif // VIDEOHEADER_H
