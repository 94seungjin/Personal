#include <stdlib.h>

#include "cctv/Client.h"
#include "test/fixture/Fixture.cpp"

int main(void)
{
	cctv::Client client(fixture::cctv1.ip, fixture::cctv1.port);
	client.Start();

	return EXIT_SUCCESS;
}