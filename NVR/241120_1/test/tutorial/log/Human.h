#ifndef HUMAN_H
#define HUMAN_H

#include <string>

#include "common/log/Logger.h"

namespace test_logger
{
	class Human
	{
	public:
		Human(std::string name);

		std::string GetName();
	private:
		static logger::Logger logger;

		std::string mName;
	};
}

#endif // HUMAN_H