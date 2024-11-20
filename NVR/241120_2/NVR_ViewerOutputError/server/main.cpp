#include "Server.hpp"
#include <iostream>

int main()
{
    const std::string metadataDir = "storage/metadata";
    const std::string rawdataDir = "storage/rawdata";
    const int port = 12345;

    try
	{
        Server server(port, metadataDir, rawdataDir);
        server.Start();
    }
	catch (const std::exception& e)
	{
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}

