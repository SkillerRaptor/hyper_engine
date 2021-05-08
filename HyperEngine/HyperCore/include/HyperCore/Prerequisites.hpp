#pragma once

#include <cstdint>

#define HYPERENGINE_EXPAND_MACRO(x) x
#define HYPERENGINE_STRINGIFY_MACRO(x) #x

namespace HyperCore
{
	static constexpr uint8_t Bit8(uint8_t shift)
	{
		return static_cast<uint8_t>(1) << shift;
	}
	
	static constexpr uint16_t Bit16(uint16_t shift)
	{
		return static_cast<uint16_t>(1) << shift;
	}
	
	static constexpr uint32_t Bit32(uint32_t shift)
	{
		return static_cast<uint32_t>(1) << shift;
	}
	
	static constexpr uint64_t Bit64(uint64_t shift)
	{
		return static_cast<uint64_t>(1) << shift;
	}
	
	static constexpr uint8_t Mask8(uint8_t shift)
	{
		return Bit8(shift) - static_cast<uint8_t>(1);
	}
	
	static constexpr uint16_t Mask16(uint16_t shift)
	{
		return Bit16(shift) - static_cast<uint16_t>(1);
	}
	
	static constexpr uint32_t Mask32(uint32_t shift)
	{
		return Bit32(shift) - static_cast<uint32_t>(1);
	}
	
	static constexpr uint64_t Mask64(uint64_t shift)
	{
		return Bit64(shift) - static_cast<uint64_t>(1);
	}
}
