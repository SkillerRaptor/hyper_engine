/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <fmt/ostream.h>

namespace HyperCore
{
	enum class ConstructError
	{
		None = 0,
		BadArgument,
		BadMemory,
		Incomplete,
		OutOfMemory,
		UndefinedBehaviour,
		Uninitialized
	};

	enum class RuntimeError
	{
		None = 0,
		OutOfBounds,
		Uninitialized,
		UndefinedBehaviour
	};
	
	auto operator<<(std::ostream& ostream, const ConstructError& construct_error) -> std::ostream&;
	auto operator<<(std::ostream& ostream, const RuntimeError& runtime_error) -> std::ostream&;
} // namespace HyperCore
