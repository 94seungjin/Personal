#ifndef FRAMEQUEUE_H
#define FRAMEQUEUE_H

#include <vector>
#include <utility>
#include "FrameHeader.h"
#include "FrameBody.h"

namespace video {

class FrameQueue
{
public:
    explicit FrameQueue(size_t capacity);
    bool IsEmpty() const;
    bool IsFull() const;
    void Push(const FrameHeader& header, const FrameBody& body);
    std::pair<FrameHeader, FrameBody> Pop();

private:
    size_t mCapacity;
    size_t mHead;
    size_t mTail;
    size_t mSize;
    std::vector<std::pair<FrameHeader, FrameBody>> mQueue;
};

} // namespace video

#endif // FRAMEQUEUE_H
