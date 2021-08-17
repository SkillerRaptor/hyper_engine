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
		MouseMovedEvent(float t_position_x, float t_position_y);
		
		float position_x;
		float position_y;
	};

	struct MouseScrolledEvent final : public IEvent
	{
		MouseScrolledEvent(float t_offset_x, float t_offset_y);
		
		float offset_x;
		float offset_y;
	};

	struct MouseButtonPressedEvent final : public IEvent
	{
		explicit MouseButtonPressedEvent(int t_button);
		
		int button;
	};

	struct MouseButtonReleasedEvent final : public IEvent
	{
		explicit MouseButtonReleasedEvent(int t_button);
		
		int button;
	};
} // namespace HyperCore
