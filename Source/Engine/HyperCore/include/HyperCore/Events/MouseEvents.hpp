/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Events/IEvent.hpp"

namespace HyperCore
{
	struct MouseMovedEvent final : public IEvent
	{
		MouseMovedEvent(float position_x, float position_y);
		
		float position_x;
		float position_y;
	};

	struct MouseScrolledEvent final : public IEvent
	{
		MouseScrolledEvent(float offset_x, float offset_y);
		
		float offset_x;
		float offset_y;
	};

	struct MouseButtonPressedEvent final : public IEvent
	{
		explicit MouseButtonPressedEvent(int button);
		
		int button;
	};

	struct MouseButtonReleasedEvent final : public IEvent
	{
		explicit MouseButtonReleasedEvent(int button);
		
		int button;
	};
} // namespace HyperCore
