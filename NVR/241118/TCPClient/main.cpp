#include "Viewer.h"
#include <iostream>

using namespace std;

int main()
{
    string serverIp = "127.0.0.1";
    int serverPort = 8080;

    Viewer viewer(serverIp, serverPort);
    viewer.Start();

    return 0;
}

