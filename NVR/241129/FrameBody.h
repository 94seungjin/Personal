#ifndef FRAMEBODY_H
#define FRAMEBODY_H

#include <vector>
#include <cstdint>

namespace video {

/**
 * @brief 프레임 본문 데이터를 저장하는 구조체
 */
struct FrameBody
{
    std::vector<uint8_t> data;

    void Serialize(std::vector<uint8_t>& buffer) const
    {
        buffer = data;
    }

    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        data = buffer;
    }
};

} // namespace video

#endif // FRAMEBODY_H
