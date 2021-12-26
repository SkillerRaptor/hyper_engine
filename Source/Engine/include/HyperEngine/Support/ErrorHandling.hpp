/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Compiler.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <cstddef>

namespace HyperEngine::Detail
{
	[[noreturn]] void unreachable(
		const char *file_name = __builtin_FILE(),
		size_t line = __builtin_LINE());
} // namespace HyperEngine::Detail

#if HYPERENGINE_DEBUG
#	define HYPERENGINE_UNREACHABLE() ::HyperEngine::Detail::unreachable()
#else
#	define HYPERENGINE_UNREACHABLE() HYPERENGINE_BUILTIN_UNREACHABLE()
#endif
