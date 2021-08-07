/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperCore
{
	struct KeyPressedEvent
	{
		int key_code;
		int repeat_count;
	};
	
	struct KeyReleasedEvent
	{
		int key_code;
	};
	
	struct KeyTypedEvent
	{
		int key_code;
	};
} // namespace HyperCore
