/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Formatter.hpp"
#include "HyperCore/Prerequisites.hpp"

#include <string_view>
#include <tuple>

namespace HyperCore
{
	class Logger
	{
	public:
		HYPERENGINE_NON_COPYABLE(Logger);
		HYPERENGINE_NON_MOVEABLE(Logger);

	public:
		enum class Level
		{
			Info,
			Warning,
			Error,
			Fatal,
			Debug
		};

	public:
		constexpr Logger() = delete;
		~Logger() = delete;

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

	private:
		template <typename... Args>
		static auto log(Level level, std::string_view format, Args&&... args) -> void
		{
			if (format.empty())
			{
				internal_log(level, format);
				return;
			}

			constexpr size_t args_count = sizeof...(Args);
			if constexpr (args_count == 0)
			{
				internal_log(level, format);
			}
			else
			{
				internal_log(level, Formatter::format(format, std::forward<Args>(args)...));
			}
		}

		static auto internal_log(Level level, std::string_view string) -> void;
	};
} // namespace HyperCore
