/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/MouseEvents.hpp"

namespace HyperCore
{
	MouseMovedEvent::MouseMovedEvent(float t_position_x, float t_position_y)
		: position_x(t_position_x)
		, position_y(t_position_y)
	{
	}

	MouseScrolledEvent::MouseScrolledEvent(float t_offset_x, float t_offset_y)
		: offset_x(t_offset_x)
		, offset_y(t_offset_y)
	{
	}

	MouseButtonPressedEvent::MouseButtonPressedEvent(int t_button)
		: button(t_button)
	{
	}

	MouseButtonReleasedEvent::MouseButtonReleasedEvent(int t_button)
		: button(t_button)
	{
	}
} // namespace HyperCore
