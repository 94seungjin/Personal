#include "Viewer.h"

Viewer::Viewer(const std::string &serverIp, int serverPort)
    : mServerIp(serverIp), mServerPort(serverPort), mSocket(-1), mWidth(0), mHeight(0) {}

Viewer::~Viewer()
{
    cleanup();
}

bool Viewer::ConnectToServer()
{
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0)
    {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    sockaddr_in serv_addr = {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(mServerPort);

    if (inet_pton(AF_INET, mServerIp.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid server address!" << std::endl;
        return false;
    }

    if (connect(mSocket, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection to server failed!" << std::endl;
        return false;
    }

    std::cout << "Connected to server!" << std::endl;

    // 메타데이터 수신 (해상도)
    if (recv(mSocket, &mWidth, sizeof(mWidth), 0) <= 0 ||
        recv(mSocket, &mHeight, sizeof(mHeight), 0) <= 0)
    {
        std::cerr << "Error receiving video metadata!" << std::endl;
        return false;
    }

    std::cout << "Video resolution: " << mWidth << "x" << mHeight << std::endl;
    return true;
}

void Viewer::receiveAndProcessVideo()
{
    const size_t frameSize = mWidth * mHeight * 3; // RGB 데이터 크기
    std::vector<char> buffer(frameSize);

    cv::VideoWriter writer;
    bool isWriterInitialized = false;

    while (true)
    {
        size_t bytesRead = 0;

        // 프레임 데이터 수신
        while (bytesRead < frameSize)
        {
            int result = recv(mSocket, buffer.data() + bytesRead, frameSize - bytesRead, 0);
            if (result <= 0)
            {
                std::cerr << "Connection closed or error receiving frame data." << std::endl;
                return;
            }
            bytesRead += result;
        }

        // Raw Data를 OpenCV Mat로 변환
        cv::Mat frame(mHeight, mWidth, CV_8UC3, buffer.data());
        if (frame.empty())
        {
            std::cerr << "Failed to create frame." << std::endl;
            continue;
        }

        // 렌더링
        cv::imshow("Video Stream", frame);

        // 재인코딩: MP4로 저장 (선택 사항)
        if (!isWriterInitialized)
        {
            writer.open("output.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                        30, cv::Size(mWidth, mHeight));
            if (!writer.isOpened())
            {
                std::cerr << "Failed to initialize video writer." << std::endl;
            }
            isWriterInitialized = true;
        }

        if (isWriterInitialized)
        {
            writer.write(frame);
        }

        // 'q' 키를 누르면 종료
        if (cv::waitKey(30) == 'q')
        {
            std::cout << "Viewer stopped by user." << std::endl;
            break;
        }
    }

    if (isWriterInitialized)
    {
        writer.release();
        std::cout << "Video saved to output.mp4." << std::endl;
    }
}

void Viewer::Start()
{
    if (!ConnectToServer())
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return;
    }

    receiveAndProcessVideo();
    cleanup();
}

void Viewer::cleanup()
{
    if (mSocket != -1)
    {
        close(mSocket);
        mSocket = -1;
        std::cout << "Connection closed." << std::endl;
    }

    cv::destroyAllWindows();
}

