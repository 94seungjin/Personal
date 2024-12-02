#include "FrameQueue.h"
#include <stdexcept>

namespace video {

FrameQueue::FrameQueue(size_t capacity)
    : mCapacity(capacity), mHead(0), mTail(0), mSize(0), mQueue(capacity) {}

bool FrameQueue::IsEmpty() const { return mSize == 0; }

bool FrameQueue::IsFull() const { return mSize == mCapacity; }

void FrameQueue::Push(const FrameHeader& header, const FrameBody& body)
{
    mQueue[mTail] = {header, body};
    mTail = (mTail + 1) % mCapacity;
    if (IsFull())
        mHead = (mHead + 1) % mCapacity;
    else
        mSize++;
}

std::pair<FrameHeader, FrameBody> FrameQueue::Pop()
{
    if (IsEmpty())
        throw std::runtime_error("Queue is empty");
    auto frame = mQueue[mHead];
    mHead = (mHead + 1) % mCapacity;
    mSize--;
    return frame;
}

} // namespace video
