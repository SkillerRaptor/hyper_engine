/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Logger.hpp"

#include <fmt/chrono.h>
#include <fmt/color.h>

namespace HyperCore
{
	auto Logger::internal_log(Logger::Level level, std::string_view string) -> void
	{
		const fmt::color level_color = [&level]()
		{
			switch (level)
			{
			case Logger::Level::Info:
				return fmt::color::white;
			case Logger::Level::Warning:
				return fmt::color::gold;
			case Logger::Level::Error:
				return fmt::color::red;
			case Logger::Level::Fatal:
				return fmt::color::crimson;
			case Logger::Level::Debug:
				return fmt::color::gray;
			default:
				return fmt::color::white;
			}
		}();

		const char* level_name = [&level]()
		{
			switch (level)
			{
			case Logger::Level::Info:
				return "Info";
			case Logger::Level::Warning:
				return "Warning";
			case Logger::Level::Error:
				return "Error";
			case Logger::Level::Fatal:
				return "Fatal";
			case Logger::Level::Debug:
				return "Debug";
			default:
				return "Undefined";
			}
		}();

		fmt::print(
			fg(level_color),
			"[ {:%H:%M:%S} | {} ] {}\n",
			fmt::localtime(std::time(nullptr)),
			level_name,
			string);
	}
} // namespace HyperCore
