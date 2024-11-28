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

    // 파일 경로 설정
    std::string filePath = "body_only_video.h264";
    std::ofstream fileStream(filePath, std::ios::binary);
    if (!fileStream.is_open())
    {
        std::cerr << "Failed to open file for writing!" << std::endl;
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

        // JPEG로 인코딩
        std::vector<uint8_t> encodedData;
        std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
        cv::imencode(".jpg", frame, encodedData, params);

        // Body 데이터만 파일에 저장
        fileStream.write(reinterpret_cast<const char*>(encodedData.data()), encodedData.size());

        // 메모리 확인
        std::cout << "Frame " << frameId++ << " saved. BodySize: " << encodedData.size() << " bytes." << std::endl;
    }

    fileStream.close();
    std::cout << "Video saved to file: " << filePath << std::endl;

    return 0;
}
