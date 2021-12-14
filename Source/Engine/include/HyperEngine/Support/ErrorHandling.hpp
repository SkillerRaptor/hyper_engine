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
	[[noreturn]] void unreachable(std::string_view file, size_t line);
} // namespace HyperEngine::Detail

#if HYPERENGINE_DEBUG
#	define HYPERENGINE_UNREACHABLE() \
		::HyperEngine::Detail::unreachable(__FILE__, __LINE__)
#else
#	define HYPERENGINE_UNREACHABLE() HYPERENGINE_BUILTIN_UNREACHABLE()
#endif
