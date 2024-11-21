#include "Storage.h"
#include <iostream>

int main()
{
    try
	{
        // Storage 객체 생성
        video::Storage storage;

        // 테스트용 비디오 경로
        const std::string videoPath = "cctv.mp4"; // 테스트하려는 비디오 파일 이름

        // 프레임을 추출하고 저장
        std::cout << "Extracting and storing frames from: " << videoPath << std::endl;
        storage.ExtractAndStoreFrames(videoPath);

        std::cout << "Frame extraction and storage complete." << std::endl;
    }
	catch (const std::exception& e)
	{
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

