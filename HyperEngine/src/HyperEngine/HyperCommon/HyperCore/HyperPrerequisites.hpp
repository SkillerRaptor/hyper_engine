#pragma once

#define HYPERENGINE_NON_COPYABLE(className)                           \
		className(const className&) = delete;                   \
		className(className&&) noexcept = delete;               \
		className& operator=(const className&) = delete;        \
		className& operator=(className&&) noexcept = delete

#define HYPERENGINE_EXPAND_MACRO(x) x
#define HYPERENGINE_STRINGIFY_MACRO(x) #x

#define HYPERENGINE_BIT8(x) (static_cast<uint8_t>(1) << (x))
#define HYPERENGINE_BIT16(x) (static_cast<uint16_t>(1) << (x))
#define HYPERENGINE_BIT32(x) (1u << (x))
#define HYPERENGINE_BIT64(x) (1ull << (x))

#define HYPERENGINE_BIT BIT32

#define HYPERENGINE_MASK8(x) (BIT8(x) - static_cast<uint8_t>(1))
#define HYPERENGINE_MASK16(x) (BIT16(x) - static_cast<uint16_t>(1))
#define HYPERENGINE_MASK32(x) (BIT32(x) - 1u)
#define HYPERENGINE_MASK64(x) (BIT64(x) - 1ull)

#define HYPERENGINE_MASK MASK32