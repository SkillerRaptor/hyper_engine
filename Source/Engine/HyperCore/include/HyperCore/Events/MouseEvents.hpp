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
	struct SMouseMovedEvent : public IEvent
	{
		float position_x;
		float position_y;
	};

	struct SMouseScrolledEvent : public IEvent
	{
		float offset_x;
		float offset_y;
	};

	struct SMouseButtonPressedEvent : public IEvent
	{
		int32_t button;
	};

	struct SMouseButtonReleasedEvent : public IEvent
	{
		int32_t button;
	};
} // namespace HyperCore
