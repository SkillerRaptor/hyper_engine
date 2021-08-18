/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperCore::Errors
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
		Uninitialized,
		UndefinedBehaviour
	};
} // namespace HyperCore::Errors
