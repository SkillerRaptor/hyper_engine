/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Formatter.hpp>
#include <string_view>

namespace HyperCore
{
	class CLogger
	{
	public:
		enum class ELevel
		{
			Info,
			Warning,
			Error,
			Fatal,
			Debug
		};

	public:
		template <typename... Args>
		static void log(ELevel level, std::string_view format, Args&&... args)
		{
			internal_log(level, CFormatter::format(format, std::forward<Args>(args)...));
		}

		template <typename... Args>
		static void info(std::string_view format, Args&&... args)
		{
			log(ELevel::Info, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void warning(std::string_view format, Args&&... args)
		{
			log(ELevel::Warning, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void error(std::string_view format, Args&&... args)
		{
			log(ELevel::Error, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void fatal(std::string_view format, Args&&... args)
		{
			log(ELevel::Fatal, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void debug(std::string_view format, Args&&... args)
		{
			log(ELevel::Debug, format, std::forward<Args>(args)...);
		}

	private:
		static void internal_log(ELevel level, std::string_view string);
	};
} // namespace HyperCore
