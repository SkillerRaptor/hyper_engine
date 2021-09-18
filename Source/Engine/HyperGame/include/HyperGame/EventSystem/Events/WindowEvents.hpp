/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperEngine
{
	struct WindowCloseEvent
	{
	};

	struct WindowResizeEvent
	{
		int32_t width;
		int32_t height;
	};

	struct WindowFramebufferResizeEvent
	{
		int32_t width;
		int32_t height;
	};

	struct WindowFocusEvent
	{
	};

	struct WindowLostFocusEvent
	{
	};

	struct WindowMovedEvent
	{
		int position_x;
		int position_y;
	};
} // namespace HyperEngine
