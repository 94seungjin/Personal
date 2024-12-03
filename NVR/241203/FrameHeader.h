#ifndef FRAMEHEADER_H
#define FRAMEHEADER_H

#include <cstdint>
#include <vector>
#include "Serialize.h"

namespace video {

/**
 * @brief 개별 비디오 프레임의 메타데이터를 저장하는 구조체
 */
struct FrameHeader
{
    uint32_t frameId;
    uint32_t bodySize;
    char timestamp[20]; // "YYYYMMDD_HHMMSS.sss"

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

#endif // FRAMEHEADER_H
