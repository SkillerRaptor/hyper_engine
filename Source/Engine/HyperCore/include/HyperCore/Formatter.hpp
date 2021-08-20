/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Prerequisites.hpp"

#include <fmt/format.h>

namespace HyperCore
{
	class Formatter
	{
	public:
		HYPERENGINE_SINGLETON(Formatter);

	public:
		template <typename... Args>
		static auto format(std::string_view format, Args&&... args) -> std::string
		{
			// TODO(SkillerRaptor): Replacing fmt::format with own implementation
			
			if (format.empty())
			{
				return format.data();
			}
			
			return fmt::format(format, std::forward<Args>(args)...);
		}
	};
} // namespace HyperCore
