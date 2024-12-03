#ifndef FRAMEQUEUE_H
#define FRAMEQUEUE_H

#include <vector>
#include <stdexcept>

template <typename T>
class FrameQueue {
public:
    explicit FrameQueue(size_t capacity)
        : mCapacity(capacity), mHead(0), mTail(0), mSize(0), mQueue(capacity) {}

    bool IsEmpty() const { return mSize == 0; }
    bool IsFull() const { return mSize == mCapacity; }

    void Push(const T& item)
    {
        mQueue[mTail] = item;
        mTail = (mTail + 1) % mCapacity;
        if (IsFull())
            mHead = (mHead + 1) % mCapacity;
        else
            mSize++;
    }

    T Pop()
    {
        if (IsEmpty())
            throw std::runtime_error("Queue is empty");
        T item = mQueue[mHead];
        mHead = (mHead + 1) % mCapacity;
        mSize--;
        return item;
    }

private:
    size_t mCapacity;
    size_t mHead;
    size_t mTail;
    size_t mSize;
    std::vector<T> mQueue;
};

#endif // FRAMEQUEUE_H
