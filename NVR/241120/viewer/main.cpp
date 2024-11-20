#include <stdlib.h>

#include "viewer/Server.h"
#include "test/fixture/Fixture.h"

int main(void)
{
	viewer::Server server(viewer::Server::PORT);
	server.Start();

	return EXIT_SUCCESS;
}