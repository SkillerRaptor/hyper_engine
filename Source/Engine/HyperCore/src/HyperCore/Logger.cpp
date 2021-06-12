/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <fmt/color.h>
#include <fmt/chrono.h>

namespace HyperCore
{
	void CLogger::internal_log(CLogger::ELevel level, const std::string& string)
	{
		const fmt::color level_color = [&level]()
		{
			switch (level)
			{
			case CLogger::ELevel::Info:
				return fmt::color::white;
			case CLogger::ELevel::Warning:
				return fmt::color::gold;
			case CLogger::ELevel::Error:
				return fmt::color::red;
			case CLogger::ELevel::Fatal:
				return fmt::color::crimson;
			case CLogger::ELevel::Debug:
				return fmt::color::gray;
			default:
				return fmt::color::white;
			}
		}();
		
		const char* level_name = [&level]()
		{
			switch (level)
			{
			case CLogger::ELevel::Info:
				return "Info";
			case CLogger::ELevel::Warning:
				return "Warning";
			case CLogger::ELevel::Error:
				return "Error";
			case CLogger::ELevel::Fatal:
				return "Fatal";
			case CLogger::ELevel::Debug:
				return "Debug";
			default:
				return "Undefined";
			}
		}();
		
		fmt::print(fg(level_color), "[ {:%H:%M:%S} | {} ] {}\n", fmt::localtime(std::time(nullptr)), level_name, string);
	}
}
