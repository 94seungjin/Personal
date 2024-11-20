#include "Viewer.hpp"
#include <iostream>

int main()
{
    const std::string serverIP = "127.0.0.1"; // 서버 IP 주소
    const int serverPort = 12345; // 서버 포트 번호

    try
	{
        Viewer viewer(serverIP, serverPort);
        viewer.Start();
    }
	catch (const std::exception& e)
	{
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

