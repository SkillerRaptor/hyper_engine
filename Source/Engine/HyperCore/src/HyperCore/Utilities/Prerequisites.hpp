/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#define HYPERENGINE_STRINGIFY_HELPER(x) #x
#define HYPERENGINE_STRINGIFY(x) HYPERENGINE_STRINGIFY_HELPER(x)

#define HYPERENGINE_NOT_USED(x) ((void) (x))

namespace HyperCore
{
	static constexpr uint8_t Bit8(uint8_t shift) noexcept
	{
		return static_cast<uint8_t>(1) << shift;
	}

	static constexpr uint16_t Bit16(uint16_t shift) noexcept
	{
		return static_cast<uint16_t>(1) << shift;
	}

	static constexpr uint32_t Bit32(uint32_t shift) noexcept
	{
		return static_cast<uint32_t>(1) << shift;
	}

	static constexpr uint64_t Bit64(uint64_t shift) noexcept
	{
		return static_cast<uint64_t>(1) << shift;
	}

	static constexpr uint8_t Mask8(uint8_t shift) noexcept
	{
		return Bit8(shift) - static_cast<uint8_t>(1);
	}

	static constexpr uint16_t Mask16(uint16_t shift) noexcept
	{
		return Bit16(shift) - static_cast<uint16_t>(1);
	}

	static constexpr uint32_t Mask32(uint32_t shift) noexcept
	{
		return Bit32(shift) - static_cast<uint32_t>(1);
	}

	static constexpr uint64_t Mask64(uint64_t shift) noexcept
	{
		return Bit64(shift) - static_cast<uint64_t>(1);
	}
} // namespace HyperCore
