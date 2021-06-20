/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <fmt/format.h>
#include <string>

namespace HyperCore
{
	class CFormatter
	{
	public:
		template <typename... Args>
		static std::string format(const std::string& format, Args&&... args)
		{
			if (format.empty())
			{
				return format;
			}

			constexpr const size_t args_count = sizeof...(Args);
			if constexpr (args_count == 0)
			{
				return format;
			}

			return fmt::format(format, std::forward<Args&&>(args)...);
		}
	};
} // namespace HyperCore
