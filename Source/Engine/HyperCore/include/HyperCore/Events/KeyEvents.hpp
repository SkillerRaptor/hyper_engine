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
	struct SKeyPressedEvent : public IEvent
	{
		int32_t key_code;
		int32_t repeat_count;
	};
	
	struct SKeyReleasedEvent : public IEvent
	{
		int32_t key_code;
	};
	
	struct SKeyTypedEvent : public IEvent
	{
		int32_t key_code;
	};
} // namespace HyperCore
