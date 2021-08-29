/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperGame
{
	struct KeyPressedEvent
	{
		int32_t key_code;
		int32_t repeat_count;
	};

	struct KeyReleasedEvent
	{
		int32_t key_code;
	};
} // namespace HyperGame
