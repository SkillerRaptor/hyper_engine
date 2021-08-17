/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Assertion.hpp"

#include "HyperCore/Logger.hpp"

namespace HyperCore
{
	auto assert(const char* expression, bool expression_value, SourceLocation current_location) -> void
	{
		if (expression_value)
		{
			return;
		}
		
		HyperCore::Logger::fatal("Assertion failed: {}", expression);
		HyperCore::Logger::fatal(
			"{}:{} in {}",
			current_location.file_name(),
			current_location.line(),
			current_location.function_name());
		std::abort();
	}
} // namespace HyperCore
