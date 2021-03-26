#include "Logger.hpp"

#include <fmt/color.h>
#include <fmt/chrono.h>

namespace HyperEngine
{
	void Logger::LogInternal(Logger::Level level, const std::string_view format)
	{
		fmt::color levelColor;
		std::string levelName;
		
		switch (level)
		{
		case Level::Info:
			levelColor = fmt::color::white;
			levelName = "Info";
			break;
		case Level::Warning:
			levelColor = fmt::color::gold;
			levelName = "Warning";
			break;
		case Level::Error:
			levelColor = fmt::color::red;
			levelName = "Error";
			break;
		case Level::Fatal:
			levelColor = fmt::color::crimson;
			levelName = "Fatal";
			break;
		case Level::Debug:
			levelColor = fmt::color::gray;
			levelName = "Debug";
			break;
		case Level::Trace:
			levelColor = fmt::color::yellow;
			levelName = "Trace";
			break;
		default:
			levelColor = fmt::color::white;
			levelName = "Undefined";
			break;
		}
		
		//fmt::print(fg(levelColor), "[ {:%H:%M:%S} | {} ] {}: {}\n", fmt::localtime(std::time(nullptr)), levelName, m_szName, format);
		fmt::print("[ {:%H:%M:%S} | {} ] {}: {}\n", fmt::localtime(std::time(nullptr)), levelName, m_name, format);
	}
}