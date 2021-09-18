/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Assertion.hpp"

#include "HyperCore/Logger.hpp"

namespace HyperEngine
{
	auto assert(const char* expression, const bool expression_value, const SourceLocation current_location) -> void
	{
		if (expression_value)
		{
			return;
		}
		
		Logger::fatal("Assertion failed: {}", expression);
		Logger::fatal(
			"{}:{} in {}",
			current_location.file_name(),
			current_location.line(),
			current_location.function_name());

		std::abort();
	}
} // namespace HyperEngine
