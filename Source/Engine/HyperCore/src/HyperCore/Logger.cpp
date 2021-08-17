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
		const auto level_color = [&level]() -> fmt::color
		{
			switch (level)
			{
			case Logger::Level::info:
				return fmt::color::white;
			case Logger::Level::warning:
				return fmt::color::gold;
			case Logger::Level::error:
				return fmt::color::red;
			case Logger::Level::fatal:
				return fmt::color::crimson;
			case Logger::Level::debug:
				return fmt::color::gray;
			default:
				return fmt::color::white;
			}
		}();

		const auto level_name = [&level]() -> const char*
		{
			switch (level)
			{
			case Logger::Level::info:
				return "Info";
			case Logger::Level::warning:
				return "Warning";
			case Logger::Level::error:
				return "Error";
			case Logger::Level::fatal:
				return "Fatal";
			case Logger::Level::debug:
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
