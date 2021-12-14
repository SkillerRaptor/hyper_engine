/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Support/ErrorHandling.hpp"

#include "HyperEngine/Logger.hpp"

#include <iostream>

namespace HyperEngine::Detail
{
	void unreachable(std::string_view file, size_t line)
	{
		Logger::error("unreachable code executed at {}:{}\n", file, line);
		std::abort();

		HYPERENGINE_BUILTIN_UNREACHABLE();
	}
} // namespace HyperEngine::Detail
