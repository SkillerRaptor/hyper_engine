/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Prerequisites.hpp"

#include <fmt/format.h>

namespace HyperCore::Formatter
{
	template <typename... Args>
	auto format(std::string_view format, Args&&... args) -> std::string
	{
		constexpr size_t args_count = sizeof...(args);
		if constexpr (args_count == 0)
		{
			return format.data();
		}

		if (format.empty())
		{
			return format.data();
		}

		// TODO(SkillerRaptor): Replacing fmt::format with own implementation
		return fmt::format(format, std::forward<Args>(args)...);
	}
} // namespace HyperCore::Formatter
