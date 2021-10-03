/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Core/Logger.hpp"

#include <fmt/chrono.h>
#include <fmt/color.h>

namespace HyperEngine
{
#if HYPERENGINE_DEBUG
	CLogger::Level CLogger::s_log_level = CLogger::Level::Trace;
#else
	CLogger::Level CLogger::s_log_level = CLogger::Level::Fatal;
#endif

	auto CLogger::internal_log(const CLogger::Level level, const std::string_view string) -> void
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
			"{} | {}: {}\n",
			fmt::format(fmt::fg(fmt::color::dark_gray), "{:%H:%M:%S}", fmt::localtime(std::time(nullptr))),
			fmt::format(fmt::fg(level_color), level_name),
			string);
	}

	auto CLogger::set_level(const CLogger::Level log_level) -> void
	{
		s_log_level = log_level;
	}

	auto CLogger::level() -> CLogger::Level
	{
		return s_log_level;
	}
} // namespace HyperEngine
