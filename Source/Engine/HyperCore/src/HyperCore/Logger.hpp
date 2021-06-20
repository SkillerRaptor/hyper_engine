/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Formatter.hpp>
#include <string>

namespace HyperCore
{
	class CLogger
	{
	private:
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
		static void log(ELevel level, const std::string& format, Args&&... args)
		{
			internal_log(level, CFormatter::format(format, std::forward<Args&&>(args)...));
		}

		template <typename... Args>
		static void info(const std::string& format, Args&&... args)
		{
			log(ELevel::Info, format, std::forward<Args&&>(args)...);
		}

		template <typename... Args>
		static void warning(const std::string& format, Args&&... args)
		{
			log(ELevel::Warning, format, std::forward<Args&&>(args)...);
		}

		template <typename... Args>
		static void error(const std::string& format, Args&&... args)
		{
			log(ELevel::Error, format, std::forward<Args&&>(args)...);
		}

		template <typename... Args>
		static void fatal(const std::string& format, Args&&... args)
		{
			log(ELevel::Fatal, format, std::forward<Args&&>(args)...);
		}

		template <typename... Args>
		static void debug(const std::string& format, Args&&... args)
		{
			log(ELevel::Debug, format, std::forward<Args&&>(args)...);
		}

	private:
		static void internal_log(ELevel level, const std::string& string);
	};
} // namespace HyperCore
