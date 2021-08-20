/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperMath/Types.hpp"

#include <cstddef>
#include <cstdint>
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
	static constexpr auto min(T a, T b) noexcept -> T
	{
		return b < a ? b : a;
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto max(T a, T b) noexcept -> T
	{
		return a < b ? b : a;
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto abs(T x) noexcept -> T
	{
		return x >= 0 ? x : -x;
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto ceil(T x) noexcept -> T
	{
		if (is_nan(x))
		{
			return std::numeric_limits<T>::quiet_NaN();
		}

		if (!is_finite(x))
		{
			return x;
		}

		if (std::numeric_limits<T>::epsilon() > HyperMath::abs(x))
		{
			return x;
		}

		auto x_whole = static_cast<int64_t>(x);
		auto remainder = (x > static_cast<T>(0)) && (x > static_cast<T>(x_whole));

		return static_cast<T>(x_whole + remainder);
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto floor(T x) noexcept -> T
	{
		if (is_nan(x))
		{
			return std::numeric_limits<T>::quiet_NaN();
		}

		if (!is_finite(x))
		{
			return x;
		}

		if (std::numeric_limits<T>::epsilon() > HyperMath::abs(x))
		{
			return x;
		}

		auto x_whole = static_cast<int64_t>(x);
		auto remainder = (x < static_cast<T>(0)) && (x < static_cast<T>(x_whole));

		return static_cast<T>(x_whole - remainder);
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto midpoint(T a, T b) noexcept -> T
	{
		if constexpr (std::is_integral_v<T>)
		{
			using U = std::make_unsigned_t<T>;

			int sign = 1;

			U first = static_cast<U>(a);
			U second = static_cast<U>(b);
			if (a > b)
			{
				sign = -1;
				first = static_cast<U>(b);
				second = static_cast<U>(a);
			}

			return a + sign * static_cast<T>(static_cast<U>(second - first) / 2);
		}
		else
		{
			constexpr T low = std::numeric_limits<T>::min() * 2;
			constexpr T high = std::numeric_limits<T>::max() / 2;
			const T absolute_a = HyperMath::abs(a);
			const T absolute_b = HyperMath::abs(b);

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

	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	static constexpr auto factorial(T n) noexcept -> T
	{
		if (n <= 1)
		{
			return 1;
		}

		auto value = T(1);

		size_t counter = 0;
		while (counter < static_cast<size_t>(n))
		{
			value *= static_cast<T>(counter + 1);
			++counter;
		}

		return value;
	}

	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	static constexpr auto pow(T x, T n) noexcept -> T
	{
		if (n <= 0)
		{
			return 1;
		}

		auto value = T(1);

		size_t counter = 0;
		while (counter < static_cast<size_t>(n))
		{
			value *= x;
			++counter;
		}

		return value;
	}
} // namespace HyperMath
