/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Compilers.hpp"
#include "HyperCore/Hasher.hpp"

#include <cstdint>

namespace HyperCore
{
	template <typename T>
	class EventType
	{
	public:
		static constexpr auto id() -> uint32_t
		{
			constexpr auto value = Hasher::hash_crc_32(HYPERENGINE_FUNCTION_SIGNATURE);
			return value;
		}
	};
} // namespace HyperCore
