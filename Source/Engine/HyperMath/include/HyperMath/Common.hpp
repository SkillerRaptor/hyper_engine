/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <limits>
#include <type_traits>

#ifdef min
#	undef min
#endif

#ifdef max
#	undef max
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

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto midpoint(T a, T b) -> T
	{
		if constexpr (std::is_integral_v<T>)
		{
			using U = std::make_unsigned_t<T>;

			int sign = 1;
			
			U first = a;
			U second = b;
			if (a > b)
			{
				sign = -1;
				first = b;
				second = a;
			}

			return a + sign * static_cast<T>(static_cast<U>(second - first) / 2);
		}
		else
		{
			constexpr T low = std::numeric_limits<T>::min() * 2;
			constexpr T high = std::numeric_limits<T>::max() * 2;
			const T absolute_a = HyperMath::abs<T>(a);
			const T absolute_b = HyperMath::abs<T>(b);
			
			if (absolute_a <= high && absolute_b <= high)
			{
				return (a + b) / 2;
			}
			
			if (absolute_a < low)
			{
				return a + b / 2;
			}
			
			if (absolute_b < low)
			{
				return a / 2 + b;
			}
			
			return a / 2 + b / 2;
		}
	}
} // namespace HyperMath
