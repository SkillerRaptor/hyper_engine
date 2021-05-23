#include <Core/Logger.hpp>

#include <fmt/color.h>
#include <fmt/chrono.h>

namespace Core
{
	void Logger::InternalLog(Logger::Level level, const std::string& string)
	{
		fmt::color levelColor{ fmt::color::white };
		std::string levelName{ "" };
		
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
		default:
			levelColor = fmt::color::white;
			levelName = "Undefined";
			break;
		}
		
		fmt::print("[ {:%H:%M:%S} | {} ] {}\n", fmt::localtime(std::time(nullptr)), levelName, string);
	}
}
