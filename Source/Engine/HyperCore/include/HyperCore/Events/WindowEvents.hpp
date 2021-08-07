/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperCore
{
	struct WindowCloseEvent
	{
	};

	struct WindowResizeEvent
	{
		int width;
		int height;
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
} // namespace HyperCore
