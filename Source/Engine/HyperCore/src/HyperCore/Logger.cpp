/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Logger.hpp"

#include "HyperCore/Prerequisites.hpp"

#include <fmt/chrono.h>
#include <fmt/color.h>

namespace HyperCore
{
#if HYPERENGINE_DEBUG
	Logger::Level Logger::s_log_level = Logger::Level::Trace;
#else
	Logger::Level Logger::s_log_level = Logger::Level::Fatal;
#endif

	auto Logger::internal_log(Level level, std::string_view string) -> void
	{
		const auto level_color = [&level]() -> fmt::color
		{
			switch (level)
			{
			case Level::Info:
				return fmt::color::white;
			case Level::Warning:
				return fmt::color::gold;
			case Level::Error:
				return fmt::color::red;
			case Level::Fatal:
				return fmt::color::crimson;
			case Level::Debug:
				return fmt::color::gray;
			default:
				return fmt::color::white;
			}
		}();

		const auto level_name = [&level]() -> const char*
		{
			switch (level)
			{
			case Level::Info:
				return "Info";
			case Level::Warning:
				return "Warning";
			case Level::Error:
				return "Error";
			case Level::Fatal:
				return "Fatal";
			case Level::Debug:
				return "Debug";
			default:
				return "Undefined";
			}
		}();

		fmt::print(
			fmt::fg(level_color),
			"[ {:%H:%M:%S} | {} ] {}\n",
			fmt::localtime(std::time(nullptr)),
			level_name,
			string);
	}
} // namespace HyperCore
