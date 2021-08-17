/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperCore
{
	enum class ConstructError
	{
		none = 0,
		incomplete,
		out_of_memory,
		undefined_behaviour
	};
} // namespace HyperCore
