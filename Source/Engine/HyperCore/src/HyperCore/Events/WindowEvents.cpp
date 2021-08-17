/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/WindowEvents.hpp"

namespace HyperCore
{
	WindowResizeEvent::WindowResizeEvent(int width, int height)
		: width(width)
		, height(height)
	{
	}

	WindowFramebufferResizeEvent::WindowFramebufferResizeEvent(int width, int height)
		: width(width)
		, height(height)
	{
	}

	WindowMovedEvent::WindowMovedEvent(int position_x, int position_y)
		: position_x(position_x)
		, position_y(position_y)
	{
	}
} // namespace HyperCore
