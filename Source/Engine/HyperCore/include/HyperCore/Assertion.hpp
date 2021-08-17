/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Prerequisites.hpp"
#include "HyperCore/SourceLocation.hpp"

#if HYPERENGINE_DEBUG
#	define HYPERENGINE_ASSERT(expression) ::HyperCore::assert(HYPERENGINE_STRINGIFY(expression), (expression))
#	define HYPERENGINE_ASSERT_NOT_REACHED() HYPERENGINE_ASSERT(false)
#else
#	define HYPERENGINE_ASSERT(expression)
#	define HYPERENGINE_ASSERT_NOT_REACHED() std::abort()
#endif

namespace HyperCore
{
	auto assert(const char* expression, bool expression_value, SourceLocation current_location = SourceLocation::current()) -> void;
} // namespace HyperCore
