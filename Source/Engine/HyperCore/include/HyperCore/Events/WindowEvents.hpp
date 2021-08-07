/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperCore
{
	struct WindowCloseEvent
	{
	};

	struct WindowResizeEvent
	{
		uint32_t width;
		uint32_t height;
	};

	struct WindowFocusEvent
	{
	};

	struct WindowLostFocusEvent
	{
	};

	struct WindowMovedEvent
	{
		uint32_t position_x;
		uint32_t position_y;
	};
} // namespace HyperCore
