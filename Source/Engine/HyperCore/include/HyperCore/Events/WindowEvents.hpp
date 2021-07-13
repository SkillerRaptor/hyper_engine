/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <HyperCore/Events/IEvent.hpp>

namespace HyperCore
{
	struct SWindowCloseEvent : public IEvent
	{
	};

	struct SWindowResizeEvent : public IEvent
	{
		uint32_t width;
		uint32_t height;
	};

	struct SWindowFocusEvent : public IEvent
	{
	};

	struct SWindowLostFocusEvent : public IEvent
	{
	};

	struct SWindowMovedEvent : public IEvent
	{
		uint32_t position_x;
		uint32_t position_y;
	};
} // namespace HyperCore
