/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Prerequisites.hpp"

#include <fmt/format.h>

#include <cstdint>
#include <string_view>

namespace HyperEngine
{
	class CLogger
	{
	public:
		HYPERENGINE_MAKE_SINGLETON(CLogger);

	public:
		enum class ELevel : uint8_t
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
		static auto info(const std::string_view format, Args&&... args) -> void
		{
			log(ELevel::Info, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto warning(const std::string_view format, Args&&... args) -> void
		{
			log(ELevel::Warning, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto error(const std::string_view format, Args&&... args) -> void
		{
			log(ELevel::Error, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto fatal(const std::string_view format, Args&&... args) -> void
		{
			log(ELevel::Fatal, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto debug(const std::string_view format, Args&&... args) -> void
		{
			log(ELevel::Debug, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static auto trace(const std::string_view format, Args&&... args) -> void
		{
			log(ELevel::Trace, format, std::forward<Args>(args)...);
		}

		static auto set_level(const ELevel log_level) -> void;
		static auto level() -> ELevel;

	private:
		static auto internal_log(const ELevel level, const std::string_view string) -> void;

		template <typename... Args>
		static auto log(const ELevel log_level, const std::string_view format, Args&&... args) -> void
		{
			if (log_level > s_log_level)
			{
				return;
			}

			constexpr size_t args_count = sizeof...(Args);
			if constexpr (args_count == 0)
			{
				internal_log(log_level, format);
			}
			else
			{
				internal_log(log_level, fmt::format(format, std::forward<Args>(args)...));
			}
		}

	private:
		static ELevel s_log_level;
	};
} // namespace HyperEngine
