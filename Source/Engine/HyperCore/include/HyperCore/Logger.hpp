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
		HYPERENGINE_SINGLETON(Logger);

	public:
		enum class Level
		{
			Info = 0,
			Warning,
			Error,
			Fatal,
			Debug,
			Trace
		};

	public:
		template <typename... Args>
		static auto info(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::Info, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto warning(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::Warning, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto error(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::Error, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto fatal(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::Fatal, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto debug(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::Debug, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto trace(std::string_view format, Args&&... args) -> void
		{
			Logger::log(Level::Trace, format, std::forward<Args>(args)...);
		}

	private:
		template <typename... Args>
		static auto log(Level log_level, std::string_view format, Args&&... args) -> void
		{
			if (log_level > s_log_level)
			{
				return;
			}
			
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
		
	private:
		static Level s_log_level;
	};
} // namespace HyperCore
