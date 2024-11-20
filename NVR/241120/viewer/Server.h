#ifndef SERVER_H
#define SERVER_H

#include "common/log/Logger.h"
#include "storage/Storage.h"
#include <string>

namespace viewer 
{
	class Server
	{
	public:
		enum { PORT = 12345 };
		
		Server() = delete;
		Server(int port, const std::string& metadataDir, const std::string& rawdataDir);
		~Server() = default;

		void Start();

	private:
		enum { BACK_LOG = 3 };
		static logger::Logger logger;

		int mPort;
		int mServerSocketFd;

		void setupServer();
		void streaming(int socketFd);

		Storage storage;
	};
}

#endif /* SERVER_H */
