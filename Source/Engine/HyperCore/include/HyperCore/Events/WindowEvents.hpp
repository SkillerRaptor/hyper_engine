/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Events/IEvent.hpp"

namespace HyperCore
{
	struct WindowCloseEvent final : public IEvent
	{
	};

	struct WindowResizeEvent final : public IEvent
	{
		WindowResizeEvent(int width, int height);
		
		int width;
		int height;
	};

	struct WindowFramebufferResizeEvent final : public IEvent
	{
		WindowFramebufferResizeEvent(int width, int height);
		
		int width;
		int height;
	};

	struct WindowFocusEvent final : public IEvent
	{
	};

	struct WindowLostFocusEvent final : public IEvent
	{
	};

	struct WindowMovedEvent final : public IEvent
	{
		WindowMovedEvent(int position_x, int position_y);
		
		int position_x;
		int position_y;
	};
} // namespace HyperCore
