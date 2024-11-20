#include "Server.hpp"

using namespace std;

int main()
{
    int port = 8080;
    string videoFile = "storage/cctv.mp4";

    Server server(port, videoFile);
    server.Start();

    return 0;
}
