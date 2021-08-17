/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/KeyEvents.hpp"

namespace HyperCore
{
	KeyPressedEvent::KeyPressedEvent(int t_key_code, int t_repeat_count)
		: key_code(t_key_code)
		, repeat_count(t_repeat_count)
	{
	}

	KeyReleasedEvent::KeyReleasedEvent(int t_key_code)
		: key_code(t_key_code)
	{
	}
} // namespace HyperCore
