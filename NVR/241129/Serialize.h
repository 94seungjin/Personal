#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <vector>
#include <stdexcept>
#include <cstring>
#include <type_traits>

namespace utils {

/**
 * @brief 객체를 바이너리 데이터로 직렬화
 * @param object 직렬화할 객체
 * @param buffer 직렬화된 데이터를 저장할 벡터
 */
template <typename T>
void Serialize(const T& object, std::vector<uint8_t>& buffer)
{
    static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
    buffer.resize(sizeof(T));
    std::memcpy(buffer.data(), &object, sizeof(T));
}

/**
 * @brief 바이너리 데이터를 객체로 역직렬화
 * @param buffer 직렬화된 데이터를 담고 있는 벡터
 * @param object 역직렬화된 데이터를 저장할 객체
 */
template <typename T>
void Deserialize(const std::vector<uint8_t>& buffer, T& object)
{
    static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
    if (buffer.size() < sizeof(T))
    {
        throw std::runtime_error("Buffer size is too small to deserialize object");
    }
    std::memcpy(&object, buffer.data(), sizeof(T));
}

} // namespace utils

#endif // SERIALIZE_H
