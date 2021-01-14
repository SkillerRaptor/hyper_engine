#include "Logger.hpp"

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

	void Logger::Print(Level level, const std::string& format)
	{
		PrintInternal(level, format);
	}

	void Logger::PrintInternal(Level level, const std::string& message)
	{
		fmt::color levelColor;
		switch (level)
		{
		case Level::HP_LEVEL_INFO:
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
		default:
			levelColor = fmt::color::white;
			break;
		}

		fmt::print(fg(levelColor), "[{:%H:%M:%S}] {} {}: {}\n", fmt::localtime(std::time(nullptr)), m_Name, ConvertLevelToString(level), message);
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

	void Logger::SetName(const std::string& name)
	{
		m_Name = name;
	}

	const std::string& Logger::GetName() const
	{
		return m_Name;
	}

	void Logger::SetLevel(Logger::Level level)
	{
		m_Level = level;
	}

	const Logger::Level Logger::GetLevel() const
	{
		return m_Level;
	}
}
