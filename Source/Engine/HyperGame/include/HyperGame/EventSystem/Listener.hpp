/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperEngine
{
	using Listener = uint64_t;

	struct ListenerTraits
	{
		using ListenerType = uint64_t;
		using EventType = uint32_t;
		using InternalListenerType = uint32_t;

		static constexpr size_t listener_mask{ 0xFFFFFFFF };
		static constexpr size_t listener_shift{ 32U };
	};
} // namespace HyperEngine
