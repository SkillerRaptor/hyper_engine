/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/MouseEvents.hpp"

namespace HyperCore
{
	MouseMovedEvent::MouseMovedEvent(float position_x, float position_y)
		: position_x(position_x)
		, position_y(position_y)
	{
	}

	MouseScrolledEvent::MouseScrolledEvent(float offset_x, float offset_y)
		: offset_x(offset_x)
		, offset_y(offset_y)
	{
	}

	MouseButtonPressedEvent::MouseButtonPressedEvent(int button)
		: button(button)
	{
	}

	MouseButtonReleasedEvent::MouseButtonReleasedEvent(int button)
		: button(button)
	{
	}
} // namespace HyperCore
