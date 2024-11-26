#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include "Storage.h"

// 모의 Server 역할
class Server
{
    public:
        void ProcessData(const video::FrameHeader& frameHeader, const video::FrameBody& frameBody) 
		{
        // Frame 정보 로그로 저장
        std::cout << "[Server] Received Frame: " << frameHeader.FrameNumber 
                  << " at " << frameHeader.FrameTimestamp << std::endl;

        // FrameBody 데이터를 확인 (로그 용도)
        std::cout << "[Server] Frame data size: " << frameBody.RawData.size() << " bytes\n";

        // Viewer 역할 (이미지 디스플레이)
        cv::Mat frame(frameHeader.Height, frameHeader.Width, CV_8UC3, (void*)frameBody.RawData.data());
        if (!frame.empty())
        {
            cv::imshow("Viewer", frame);
            cv::waitKey(1); // 1ms 대기
        }
    }
};

// Edge 역할: 카메라 캡처 및 데이터 생성
void EdgeFunction(video::Storage& storage)
{
    cv::VideoCapture cap(0, cv::CAP_V4L2); // Open default camera
    if (!cap.isOpened())
    {
        std::cerr << "[Edge] Error: Unable to open camera\n";
        return;
    }
	
	// 카메라 해상도, 프레임 조절
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 10); 

    int frameNumber = 0;
    while (true)
    {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) continue;

        // Frame 정보를 JSON으로 직렬화
        uint64_t timestamp = static_cast<uint64_t>(cv::getTickCount());
        video::FrameHeader frameHeader
        {
            timestamp,
            static_cast<uint32_t>(frameNumber++), // 명시적 형변환 추가
            static_cast<uint32_t>(frame.cols),   // 명시적 형변환 추가
            static_cast<uint32_t>(frame.rows),   // 명시적 형변환 추가
            1
        };

        // 바이너리 데이터 생성
        video::FrameBody frameBody;
        frameBody.RawData.assign(frame.datastart, frame.dataend);

        // Simulate sending data to Client (direct call for simplicity)
        std::cout << "[Edge] Sending frame " << frameHeader.FrameNumber << " to Client\n";

        // Client 역할을 대신하여 Storage에 직접 전달
        storage.SaveFrameHeader(frameHeader);
        storage.SaveFrameBody(frameBody);

        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // Simulate ~30fps
    }
}
// Main 함수
int main()
{
    // Storage 초기화
    video::Storage storage("data_storage", 10);
    Server server;

    // Edge에서 데이터를 생성하고 Storage로 전달
    std::thread edgeThread(EdgeFunction, std::ref(storage));

    // Server에서 Storage 데이터를 처리
    while (true)
    {
        video::FrameHeader frameHeader;
        video::FrameBody frameBody;

        // Storage에서 검색된 데이터 전달받기
        if (storage.SearchFrameHeader(0, frameHeader) && storage.SearchFrameBody(0, frameBody))
        {
            std::cout << "[Main] Processing frame " << frameHeader.FrameNumber << "...\n";
            server.ProcessData(frameHeader, frameBody);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // No new data
        }
    }

    edgeThread.join();
    return 0;
}
