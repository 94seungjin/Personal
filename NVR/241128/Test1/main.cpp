#include <opencv2/opencv.hpp>
#include "Storage.h"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace video;

// 타임스탬프 생성
std::string GetTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

int main()
{
    // OpenCV로 카메라 초기화
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "Failed to open camera!" << std::endl;
        return -1;
    }

    // 카메라 해상도 설정
    int width = 640;
    int height = 480;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    // VideoWriter 초기화 (H.264 코덱 사용)
    std::string outputFile = "output_video.h264";
    cv::VideoWriter writer(outputFile, cv::VideoWriter::fourcc('X', '2', '6', '4'), 15, cv::Size(width, height));

    if (!writer.isOpened())
    {
        std::cerr << "Failed to open VideoWriter!" << std::endl;
        return -1;
    }

    int frameId = 0;
    while (frameId < 300) // 300프레임을 캡처
    {
        cv::Mat frame;
        cap >> frame; // 프레임 캡처

        if (frame.empty())
        {
            std::cerr << "Empty frame captured!" << std::endl;
            break;
        }

        // H.264 스트림으로 저장
        writer.write(frame);
        std::cout << "Frame " << frameId++ << " saved to video." << std::endl;
    }

    std::cout << "Video saved to file: " << outputFile << std::endl;
    return 0;
}

