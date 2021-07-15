/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Formatter.hpp>
#include <HyperCore/Prerequisites.hpp>
#include <string_view>

namespace HyperCore
{
	class CLogger
	{
	public:
		HYPERENGINE_NON_COPYABLE(CLogger);
		HYPERENGINE_NON_MOVEABLE(CLogger);
		
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
		constexpr CLogger() = delete;
		~CLogger() = delete;
		
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
		template <typename... Args>
		static void log(ELevel level, std::string_view format, Args&&... args)
		{
			if (format.empty())
			{
				internal_log(level, format);
				return;
			}
			
			static constexpr const size_t args_count = sizeof...(Args);
			if constexpr (args_count == 0)
			{
				internal_log(level, format);
				return;
			}
			
			internal_log(level, CFormatter::format(format, std::forward<Args>(args)...));
		}
		
		static void internal_log(ELevel level, std::string_view string);
	};
} // namespace HyperCore
