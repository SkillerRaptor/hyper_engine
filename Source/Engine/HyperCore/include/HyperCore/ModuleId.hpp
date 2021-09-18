/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Compilers.hpp"
#include "HyperCore/Hasher.hpp"

#include <cstdint>

namespace HyperEngine
{
	template <typename T>
	class ModuleId
	{
	public:
		static constexpr auto id() -> uint32_t
		{
			constexpr uint32_t value = Hasher::hash_fnv_32(HYPERENGINE_FUNCTION_SIGNATURE);
			return value;
		}
	};
} // namespace HyperEngine
