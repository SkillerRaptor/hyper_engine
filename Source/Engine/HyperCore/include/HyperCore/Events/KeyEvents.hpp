/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Events/IEvent.hpp"

namespace HyperCore
{
	struct KeyPressedEvent final : public IEvent
	{
		KeyPressedEvent(int t_key_code, int t_repeat_count);
		
		int key_code;
		int repeat_count;
	};
	
	struct KeyReleasedEvent final : public IEvent
	{
		explicit KeyReleasedEvent(int t_key_code);
		
		int key_code;
	};
} // namespace HyperCore
