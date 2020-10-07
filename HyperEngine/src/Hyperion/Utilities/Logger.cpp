#include "Logger.hpp"

#include <stdarg.h>

namespace Hyperion {

	Logger::Logger(std::queue<std::pair<Level, std::string>>* messageQueue)
		: m_Name("Example Logger Name"), m_Level(Level::HP_LEVEL_ALL), m_MessageQueue(messageQueue)
	{
	}

	Logger::~Logger()
	{
	}

	void Logger::PrintInfo(const char* format)
	{
		Print(Level::HP_LEVEL_INFO, format);
	}

	void Logger::PrintTrace(const char* format)
	{
		Print(Level::HP_LEVEL_TRACE, format);
	}

	void Logger::PrintWarn(const char* format)
	{
		Print(Level::HP_LEVEL_WARN, format);
	}

	void Logger::PrintError(const char* format)
	{
		Print(Level::HP_LEVEL_ERROR, format);
	}

	void Logger::PrintFatal(const char* format)
	{
		Print(Level::HP_LEVEL_FATAL, format);
	}

	void Logger::PrintDebug(const char* format)
	{
		Print(Level::HP_LEVEL_DEBUG, format);
	}

	void Logger::Print(Level level, const char* format)
	{
		std::stringstream ss;
		ss << m_Name << " " << ConvertLevelToString(level) << ": " << format << std::endl;

		m_MessageQueue->push(std::make_pair(level, ss.str()));
	}

	std::string Logger::ConvertLevelToString(Level level)
	{
		std::string levelName;
		switch (level)
		{
		case Level::HP_LEVEL_INFO:
			levelName = "Info";
			break;
		case Level::HP_LEVEL_TRACE:
			levelName = "Trace";
			break;
		case Level::HP_LEVEL_WARN:
			levelName = "Warn";
			break;
		case Level::HP_LEVEL_ERROR:
			levelName = "Error";
			break;
		case Level::HP_LEVEL_FATAL:
			levelName = "Fatal";
			break;
		case Level::HP_LEVEL_DEBUG:
			levelName = "Debug";
			break;
		}
		return levelName;
	}

	char* Logger::ConvertNumber(unsigned int number, int base)
	{
		static char Representation[] = "0123456789ABCDEF";
		static char buffer[50];
		char* ptr;

		ptr = &buffer[49];
		*ptr = '\0';

		do
		{
			*--ptr = Representation[number % base];
			number /= base;
		} while (number != 0);

		return(ptr);
	}
}
