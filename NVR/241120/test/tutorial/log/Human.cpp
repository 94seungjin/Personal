#include "Human.h"

#include <string>

namespace test_logger
{
	logger::Logger Human::logger("Human");

	Human::Human(std::string name)
		: mName(name)
	{
	}

	std::string Human::GetName()
	{
		logger.Info("GetName() start");

		return mName;
	}
}