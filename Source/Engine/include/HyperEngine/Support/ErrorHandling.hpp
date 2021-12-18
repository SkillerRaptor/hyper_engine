/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Compiler.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <string_view>

namespace HyperEngine::Detail
{
	[[noreturn]] void unreachable(
		const char *file_name = __builtin_FILE(),
		uint32_t line = __builtin_LINE());
} // namespace HyperEngine::Detail

#if HYPERENGINE_DEBUG
#	define HYPERENGINE_UNREACHABLE() ::HyperEngine::Detail::unreachable()
#else
#	define HYPERENGINE_UNREACHABLE() HYPERENGINE_BUILTIN_UNREACHABLE()
#endif
