#ifndef VIEWER_H
#define VIEWER_H

#include <string>
#include <vector>
#include "../common/log/Logger.h"

class Viewer
{
	public:
		Viewer(const std::string& serverIP, int port);
		~Viewer();

		void Start();

	private:
		std::string serverIP;
		static logger::Logger logger;
		int port;
		int socketFd;

		void connectToServer();
		void receiveAndProcessData();
		void processMetadata(const std::string& metadata);
		void processRawData(const std::vector<uint8_t>& rawData);
};

#endif	/* VIEWER_H */
