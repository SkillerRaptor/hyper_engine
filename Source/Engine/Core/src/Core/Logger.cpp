#include "Logger.hpp"

#include <fmt/color.h>
#include <fmt/chrono.h>

namespace Core
{
	void Logger::internal_log(Logger::Level log_level, const std::string& string)
	{
		// TODO: Adding cross platform colors
		fmt::color level_color;
		std::string level_name;
		
		switch (log_level)
		{
		case Level::Info:
			level_color = fmt::color::white;
			level_name = "Info";
			break;
		case Level::Warning:
			level_color = fmt::color::gold;
			level_name = "Warning";
			break;
		case Level::Error:
			level_color = fmt::color::red;
			level_name = "Error";
			break;
		case Level::Fatal:
			level_color = fmt::color::crimson;
			level_name = "Fatal";
			break;
		case Level::Debug:
			level_color = fmt::color::gray;
			level_name = "Debug";
			break;
		default:
			level_color = fmt::color::white;
			level_name = "Undefined";
			break;
		}
		
		// TODO: Replacing with own time printing
		fmt::print("[ {:%H:%M:%S} | {} ] {}\n", fmt::localtime(std::time(nullptr)), level_name, string);
	}
}
