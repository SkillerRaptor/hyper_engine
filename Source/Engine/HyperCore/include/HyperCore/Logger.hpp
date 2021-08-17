/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Formatter.hpp"
#include "HyperCore/Prerequisites.hpp"

#include <string_view>

namespace HyperCore
{
	class Logger
	{
	public:
		HYPERENGINE_NON_COPYABLE(Logger);
		HYPERENGINE_NON_MOVABLE(Logger);

	public:
		enum class Level
		{
			info,
			warning,
			error,
			fatal,
			debug
		};

	public:
		constexpr Logger() = delete;
		~Logger() = delete;

		template <typename... Args>
		static auto info(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::info, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto warning(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::warning, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto error(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::error, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto fatal(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::fatal, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto debug(std::string_view format, Args&&... args) -> void
		{
#if HYPERENGINE_DEBUG
			Logger::log(Level::debug, format, std::forward<Args>(args)...);
#else
			(void) format;
			((void) args, ...);
#endif
		}

	private:
		template <typename... Args>
		static auto log(Level log_level, std::string_view format, Args&&... args) -> void
		{
			if (format.empty())
			{
				internal_log(log_level, format);
				return;
			}

			constexpr size_t args_count = sizeof...(Args);
			if constexpr (args_count == 0)
			{
				internal_log(log_level, format);
			}
			else
			{
				internal_log(log_level, Formatter::format(format, std::forward<Args>(args)...));
			}
		}

		static auto internal_log(Level level, std::string_view string) -> void;
	};
} // namespace HyperCore
