/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

#ifdef min
	#undef min
#endif

#ifdef max
	#undef max
#endif

namespace HyperMath
{
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	[[nodiscard]] static constexpr T abs(T x) noexcept
	{
		return x >= 0 ? x : -x;
	}
	
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	[[nodiscard]] static constexpr T min(T a, T b) noexcept
	{
		return b < a ? b : a;
	}
	
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	[[nodiscard]] static constexpr T max(T a, T b) noexcept
	{
		return a < b ? b : a;
	}
}
