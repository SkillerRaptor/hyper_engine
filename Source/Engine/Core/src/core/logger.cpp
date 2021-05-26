#include <core/logger.hpp>

#include <fmt/color.h>
#include <fmt/chrono.h>

namespace core
{
	void logger::internal_log(core::logger::level log_level, const std::string& string)
	{
		fmt::color level_color{ fmt::color::white };
		std::string level_name{ "" };
		
		switch (log_level)
		{
		case level::info:
			level_color = fmt::color::white;
			level_name = "Info";
			break;
		case level::warning:
			level_color = fmt::color::gold;
			level_name = "Warning";
			break;
		case level::error:
			level_color = fmt::color::red;
			level_name = "Error";
			break;
		case level::fatal:
			level_color = fmt::color::crimson;
			level_name = "Fatal";
			break;
		case level::debug:
			level_color = fmt::color::gray;
			level_name = "Debug";
			break;
		default:
			level_color = fmt::color::white;
			level_name = "Undefined";
			break;
		}
		
		fmt::print("[ {:%H:%M:%S} | {} ] {}\n", fmt::localtime(std::time(nullptr)), level_name, string);
	}
}
