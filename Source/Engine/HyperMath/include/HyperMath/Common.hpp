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
	static constexpr auto abs(T x) -> T
	{
		return x >= 0 ? x : -x;
	}
	
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto min(T a, T b) -> T
	{
		return b < a ? b : a;
	}
	
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto max(T a, T b) -> T
	{
		return a < b ? b : a;
	}
}
