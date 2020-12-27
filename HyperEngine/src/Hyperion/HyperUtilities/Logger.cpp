#include "Logger.hpp"

#if defined(HP_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

#include <ctime>
#include <iostream>

namespace Hyperion {

	Logger::Logger()
		: m_Name(""), m_Level(Level::HP_LEVEL_TRACE)
	{
	}

	Logger::~Logger()
	{
	}

	void Logger::PrintInfo(const char* format)
	{
		Print(Level::HP_LEVEL_INFO, format);
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

		Print(level, ss.str());
	}

	void Logger::Print(Level level, const std::string& message)
	{
	#if defined(HP_PLATFORM_WINDOWS)
		switch (level)
		{
		case Level::HP_LEVEL_INFO: default:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		case Level::HP_LEVEL_WARN:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
			break;
		case Level::HP_LEVEL_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case Level::HP_LEVEL_FATAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			break;
		case Level::HP_LEVEL_DEBUG:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		}
	#elif defined(HP_PLATFORM_LINUX)
		switch (level)
		{
		case Level::HP_LEVEL_INFO: default:
			printf("\e[97m");
			break;
		case Level::HP_LEVEL_WARN:
			printf("\e[33m");
			break;
		case Level::HP_LEVEL_ERROR:
			printf("\e[91m");
			break;
		case Level::HP_LEVEL_FATAL:
			printf("\e[31m");
			break;
		case Level::HP_LEVEL_DEBUG:
			printf("\e[37m");
			break;
		}
	#endif

		std::time_t time = std::time(0);
		struct tm localTime;
		localtime_s(&localTime, &time);

		printf("[%c%i:%c%i:%c%i] %s", (localTime.tm_hour < 10 ? '0' : '\0'), localTime.tm_hour, (localTime.tm_min < 10 ? '0' : '\0'), localTime.tm_min, (localTime.tm_sec < 10 ? '0' : '\0'), localTime.tm_sec, message.c_str());
	#if defined(HP_PLATFORM_WINDOWS)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	#elif defined(HP_PLATFORM_LINUX)
		printf("\e[39m");
	#endif
	}

	std::string Logger::ConvertLevelToString(Level level) const
	{
		std::string levelName;
		switch (level)
		{
		case Level::HP_LEVEL_INFO:
			levelName = "Info";
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


	void Logger::FormatString(std::stringstream& ss, const char* format)
	{
		ss << format;
	}

	const void Logger::SetName(std::string name)
	{
		m_Name = name;
	}

	const std::string Logger::GetName() const
	{
		return m_Name;
	}

	const void Logger::SetLevel(Level level)
	{
		m_Level = level;
	}

	const Level Logger::GetLevel() const
	{
		return m_Level;
	}
}
