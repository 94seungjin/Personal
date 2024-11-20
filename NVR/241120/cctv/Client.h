#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include "common/log/Logger.h"

namespace cctv
{
	typedef struct host {
		std::string ip;
		int port;
	} host_t;

	class Client
	{
	public:
		Client(std::string& host, int port);
		~Client();
		
		void Start();
		void Close();

	private:
		static logger::Logger logger;

		std::string& mHost;
		int mPort;
		int mSocketFd;
		bool mbClosed;

		void connectToServer();

		void receiveFrameAndSaveToFile();
	};
}
#endif /* CLIENT_H */