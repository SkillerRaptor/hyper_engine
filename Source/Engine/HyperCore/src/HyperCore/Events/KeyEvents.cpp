/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/KeyEvents.hpp"

namespace HyperCore
{
	KeyPressedEvent::KeyPressedEvent(int key_code, int repeat_count)
		: key_code(key_code)
		, repeat_count(repeat_count)
	{
	}

	KeyReleasedEvent::KeyReleasedEvent(int key_code)
		: key_code(key_code)
	{
	}
} // namespace HyperCore
