#include "Storage.hpp"

Storage::Storage(int maxSize) : mMaxSize(maxSize), mHead(0), mTail(0), mSize(0)
{
	mQueue.resize(maxSize);
}

void Storage::pushFrame(const FrameData &frame)
{
	if (isFull())
	{
		// 큐가 가득 차면 가장 오래된 데이터 덮어쓰기
		mHead = (mHead + 1) % mMaxSize;
		mSize--;	// 기존 프레임 제거
	}

	mQueue[mTail] = frame;
	mTail = (mTail + 1) % mMaxSize;
	mSize++;
}

FrameData Storage::popFrame()
{
	if (isEmpty())
	{
		throw std::runtime_error("Storgrage is empty!");
	}

	FrameData frame = mQueue[mHead];
	mHead = (mHead + 1) % mMaxSize;
	mSize--;
	return frame;
}

bool Storage::isEmpty() const
{
	return mSize == 0;
}

bool Storage::isFull() const
{
	return mSize == mMaxSize;
}

