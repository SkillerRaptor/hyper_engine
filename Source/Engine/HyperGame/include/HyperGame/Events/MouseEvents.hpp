/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperGame/MouseButton.hpp"

#include <cstdint>

namespace HyperGame
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
		MouseButton button;
	};

	struct MouseButtonReleasedEvent
	{
		MouseButton button;
	};
} // namespace HyperGame
