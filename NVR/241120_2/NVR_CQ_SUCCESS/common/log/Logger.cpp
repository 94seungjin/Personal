#include <cassert>
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>

#include "Logger.h"

using namespace std;

namespace logger
{
	Logger::Logger(const string& className)
		: mClassName(className) 
	{
		// Makefile에서 PROJECT_ROOT가 정의
        string logFilePath = string(PROJECT_ROOT) + "/storage/log.txt";
        mLogFile.open(logFilePath, ios_base::app);
		if (mLogFile.is_open() == false) {
			cerr << "Error opening log file" << endl;
		}
	}

	Logger::~Logger() 
	{
		if (mLogFile.is_open()) {
			mLogFile.close();
		}
	}

	string Logger::logLevelToString(LogLevel level) 
	{
		switch (level) {
			case LogLevel::DEBUG: 
				return "DEBUG";
			case LogLevel::INFO: 
				return "INFO";
			case LogLevel::WARNING: 
				return "WARNING";
			case LogLevel::ERROR: 
				return "ERROR";
			default:
				assert(false);
				return "UNKNOWN";
		}
	}

	void Logger::Info(const string& message) 
	{
		log(LogLevel::INFO, message);
	}

	void Logger::Debug(const string& message) 
	{
		log(LogLevel::DEBUG, message);
	}

	void Logger::Warning(const string& message) 
	{
		log(LogLevel::WARNING, message);
	}

	void Logger::Error(const string& message) 
	{
		log(LogLevel::ERROR, message);
	}

	void Logger::log(const LogLevel& level, const string& message) 
	{
		string logMessage = getTimestamp() + " [" + logLevelToString(level) + "] " + mClassName + ": " + message;

		if (mLogFile.is_open()) {
			mLogFile << logMessage << endl;
		}
		cout << logMessage << endl;
	}

	string Logger::getTimestamp() 
	{
		time_t now_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
		char buffer[20];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now_time));
		return string(buffer);
	}
}