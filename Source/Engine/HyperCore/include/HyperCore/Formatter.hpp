/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <fmt/format.h>

namespace HyperCore
{
	class Formatter
	{
	public:
		template <typename... Args>
		static auto format(std::string_view format, Args&&... args) -> std::string
		{
			// TODO: Implement own formatter
			return fmt::format(format, std::forward<Args>(args)...);
		}
	};
} // namespace HyperCore
