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
	CLogger::ELevel CLogger::s_log_level = CLogger::ELevel::Trace;
#else
	CLogger::ELevel CLogger::s_log_level = CLogger::ELevel::Fatal;
#endif

	auto CLogger::internal_log(const CLogger::ELevel level, const std::string_view string) -> void
	{
		const auto level_color = [&level]() -> fmt::color
		{
			switch (level)
			{
			case ELevel::Info:
				return fmt::color::white;
			case ELevel::Warning:
				return fmt::color::gold;
			case ELevel::Error:
			case ELevel::Fatal:
				return fmt::color::crimson;
			case ELevel::Debug:
				return fmt::color::gray;
			default:
				return fmt::color::white;
			}
		}();

		const auto level_name = [&level]() -> const char*
		{
			switch (level)
			{
			case ELevel::Info:
				return "Info";
			case ELevel::Warning:
				return "Warning";
			case ELevel::Error:
				return "Error";
			case ELevel::Fatal:
				return "Fatal";
			case ELevel::Debug:
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

	auto CLogger::set_level(const CLogger::ELevel log_level) -> void
	{
		s_log_level = log_level;
	}

	auto CLogger::level() -> CLogger::ELevel
	{
		return s_log_level;
	}
} // namespace HyperEngine
