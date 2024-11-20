#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

using namespace std;

namespace logger
{
	enum LogLevel 
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};

	class Logger 
	{
	public:
		Logger(const string& className);
		~Logger();

		void Info(const string& message);
		void Debug(const string& message);
		void Warning(const string& message);
		void Error(const string& message);

	private:
		string logLevelToString(LogLevel level);
		void log(const LogLevel& level, const string& message);
		string getTimestamp();

		string mClassName;
		ofstream mLogFile;
	};
}

#endif // LOGGER_H