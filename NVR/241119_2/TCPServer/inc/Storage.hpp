#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <queue>

struct Metadata
{
	int frameNumber;		// 프레임 번호
	std::string timestamp;	// 타임스탬프
	int width;				// 프레임 너비
	int height;				// 프레임 높이

};

struct FrameData
{
	Metadata metadata;		// 메타데이터
	cv::Mat rawData;		// 원본 프레임 데이터
};

class Storage
{
	public:
		Storage(int maxSize);
		~Storage() = default;

		void pushFrame(const FrameData &frame);		// 프레임 추가
		FrameData popFrame();						// 가장 오래된 프레임 제거 후 반환
		bool isEmpty() const;						// 큐가 비어있는지 확인
		bool isFull() const;						// 가득찼는지 확인
		
	private:
		std::vector<FrameData> mQueue;				// 원형 큐 데이터 저장
		int mMaxSize;								// 큐의 최대 크기
		int mHead;									// 큐의 시작 지점
		int mTail;									// 큐의 끝 지점
		int mSize;									// 현재 저장된 프레임 수
};

#endif	// STORAGE_CPP

