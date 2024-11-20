#include <stdlib.h>

#include "viewer/Server.h"
#include "test/fixture/Fixture.h"

int main(void)
{
    const std::string metadataDir = "/home/sj/Desktop/NVR/storage/metadata";
    const std::string rawdataDir = "/home/sj/Desktop/NVR/storage/rawdata";

    viewer::Server server(viewer::Server::PORT, metadataDir, rawdataDir);
    server.Start();

    return EXIT_SUCCESS;
}

