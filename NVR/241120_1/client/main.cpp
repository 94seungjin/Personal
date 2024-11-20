#include "Viewer.h"
#include <iostream>

int main()
{
    const std::string serverIP = "127.0.0.1";  // 서버 IP
    const int port = 12345;  // 서버 포트

    try
	{
        Viewer viewer(serverIP, port);
        viewer.Start();
    }
	catch (const std::exception& e)
	{
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

