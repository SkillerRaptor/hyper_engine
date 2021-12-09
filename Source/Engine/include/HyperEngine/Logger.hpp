/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Formatter.hpp"

#include <cstdio>
#include <sstream>
#include <string_view>

namespace HyperEngine::Logger
{
	namespace Detail
	{
		template <typename T>
		concept Printable = requires(std::ostream &ostream, T value)
		{
			ostream << value;
		};
		
		enum class Level
		{
			Info = 0,
			Warning,
			Error,
			Debug
		};

		template <Detail::Printable... Args>
		void log(Level level, std::string_view format, Args &&...args)
		{
			const std::string_view color_string = [level]()
			{
				switch (level)
				{
				case Level::Info:
					return "\033[32m";
				case Level::Warning:
					return "\033[33m";
				case Level::Error:
					return "\033[31m";
				case Level::Debug:
					return "\033[36m";
				default:
					HYPERENGINE_UNREACHABLE();
				}
			}();

			const std::string_view level_string = [level]()
			{
				switch (level)
				{
				case Level::Info:
					return "info";
				case Level::Warning:
					return "warning";
				case Level::Error:
					return "error";
				case Level::Debug:
					return "debug";
				default:
					HYPERENGINE_UNREACHABLE();
				}
			}();

			const Expected<std::string> formatted_string =
				Formatter::format(format, std::forward<Args>(args)...);
			if (formatted_string.is_error())
			{
				return;
			}

			const std::string string =
				[color_string, level_string, formatted_string]()
			{
				std::stringstream string_stream;
				string_stream << "\033[37m";
				string_stream << "hyper";
				string_stream << "\033[0m: ";
				string_stream << color_string;
				string_stream << level_string;
				string_stream << "\033[0m: ";
				string_stream << formatted_string.value();
				return string_stream.str();
			}();

			fwrite(string.c_str(), string.length(), 1, stdout);
		}
	} // namespace Detail

	template <Detail::Printable... Args>
	void info(std::string_view format, Args &&...args)
	{
		Detail::log(Detail::Level::Info, format, std::forward<Args>(args)...);
	}

	template <Detail::Printable... Args>
	void warning(std::string_view format, Args &&...args)
	{
		Detail::log(Detail::Level::Warning, format, std::forward<Args>(args)...);
	}

	template <Detail::Printable... Args>
	void error(std::string_view format, Args &&...args)
	{
		Detail::log(Detail::Level::Error, format, std::forward<Args>(args)...);
	}

	template <Detail::Printable... Args>
	void debug(std::string_view format, Args &&...args)
	{
		Detail::log(Detail::Level::Debug, format, std::forward<Args>(args)...);
	}
} // namespace HyperEngine::Logger
