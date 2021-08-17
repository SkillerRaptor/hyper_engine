/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/WindowEvents.hpp"

namespace HyperCore
{
	WindowResizeEvent::WindowResizeEvent(int t_width, int t_height)
		: width(t_width)
		, height(t_height)
	{
	}

	WindowFramebufferResizeEvent::WindowFramebufferResizeEvent(int t_width, int t_height)
		: width(t_width)
		, height(t_height)
	{
	}

	WindowMovedEvent::WindowMovedEvent(int t_position_x, int t_position_y)
		: position_x(t_position_x)
		, position_y(t_position_y)
	{
	}
} // namespace HyperCore
