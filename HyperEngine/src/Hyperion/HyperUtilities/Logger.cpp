#include "Logger.hpp"

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/chrono.h>

#include <chrono>
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
		Print(level, std::string(format));
	}

	void Logger::Print(Level level, const std::string& message)
	{
		fmt::color levelColor;
		switch (level)
		{
		case Level::HP_LEVEL_INFO: default:
			levelColor = fmt::color::white;
			break;
		case Level::HP_LEVEL_WARN:
			levelColor = fmt::color::gold;
			break;
		case Level::HP_LEVEL_ERROR:
			levelColor = fmt::color::red;
			break;
		case Level::HP_LEVEL_FATAL:
			levelColor = fmt::color::crimson;
			break;
		case Level::HP_LEVEL_DEBUG:
			levelColor = fmt::color::gray;
			break;
		}
		std::time_t time = std::time(nullptr);
		fmt::print(fg(levelColor), "[{:%H:%M:%S}] {} {}: {}\n", fmt::localtime(time), m_Name, ConvertLevelToString(level), message);
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
