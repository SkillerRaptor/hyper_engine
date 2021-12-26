/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Support/ErrorHandling.hpp"

#include "HyperEngine/Logger.hpp"

namespace HyperEngine::Detail
{
	void unreachable(const char *file_name, size_t line)
	{
		Logger::error("unreachable code executed at {}:{}\n", file_name, line);
		std::abort();

		HYPERENGINE_BUILTIN_UNREACHABLE();
	}
} // namespace HyperEngine::Detail
