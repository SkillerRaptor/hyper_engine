/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperCore
{
	struct MouseMovedEvent
	{
		float position_x;
		float position_y;
	};

	struct MouseScrolledEvent
	{
		float offset_x;
		float offset_y;
	};

	struct MouseButtonPressedEvent
	{
		int32_t button;
	};

	struct MouseButtonReleasedEvent
	{
		int32_t button;
	};
} // namespace HyperCore
