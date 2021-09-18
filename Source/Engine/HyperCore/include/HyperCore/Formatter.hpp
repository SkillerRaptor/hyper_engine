/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Assertion.hpp"

#include <fmt/format.h>

namespace HyperEngine
{
	class Formatter
	{
	public:
		template <typename... Args>
		static auto format(const std::string_view format, Args&&... args) -> std::string
		{
			HYPERENGINE_ASSERT(!format.empty());

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
	};
} // namespace HyperEngine
