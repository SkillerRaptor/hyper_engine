/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Math/Types.hpp"

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

namespace HyperEngine
{
	template <typename T>
	[[nodiscard]] constexpr auto min(const T a, const T b) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return (b < a) ? b : a;
	}

	template <typename T>
	[[nodiscard]] constexpr auto max(const T a, const T b) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return (a < b) ? b : a;
	}

	template <typename T>
	[[nodiscard]] constexpr auto abs(const T x) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return (x >= 0) ? x : -x;
	}

	template <typename T>
	[[nodiscard]] constexpr auto ceil(const T x) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		if (is_nan(x))
		{
			return std::numeric_limits<T>::quiet_NaN();
		}

		if (!is_finite(x))
		{
			return x;
		}

		if (std::numeric_limits<T>::epsilon() > abs(x))
		{
			return x;
		}

		const int64_t x_whole = static_cast<int64_t>(x);
		const bool remainder = (x > static_cast<T>(0)) && (x > static_cast<T>(x_whole));
		return static_cast<T>(x_whole + remainder);
	}

	template <typename T>
	[[nodiscard]] constexpr auto clamp(const T value, const T low, const T high) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return min(max(value, low), high);
	}

	template <typename T>
	[[nodiscard]] constexpr auto saturate(const T value) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return clamp(value, static_cast<T>(0.0F), static_cast<T>(1.0F));
	}

	template <typename T>
	[[nodiscard]] constexpr auto floor(const T x) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		if (is_nan(x))
		{
			return std::numeric_limits<T>::quiet_NaN();
		}

		if (!is_finite(x))
		{
			return x;
		}

		if (std::numeric_limits<T>::epsilon() > abs(x))
		{
			return x;
		}

		const int64_t x_whole = static_cast<int64_t>(x);
		const bool remainder = (x < static_cast<T>(0)) && (x < static_cast<T>(x_whole));
		return static_cast<T>(x_whole - remainder);
	}

	template <typename T>
	[[nodiscard]] constexpr auto midpoint(const T a, const T b) noexcept -> T
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		if constexpr (std::is_integral_v<T>)
		{
			using UnsignedT = std::make_unsigned_t<T>;

			const int sign = (a > b) ? -1 : 1;
			const UnsignedT first = static_cast<UnsignedT>((a > b) ? b : a);
			const UnsignedT second = static_cast<UnsignedT>((a > b) ? a : b);
			return a + sign * static_cast<T>(static_cast<UnsignedT>(second - first) / 2);
		}
		else
		{
			constexpr T low = std::numeric_limits<T>::min() * 2;
			constexpr T high = std::numeric_limits<T>::max() / 2;
			const T absolute_a = abs(a);
			const T absolute_b = abs(b);

			if (absolute_a <= high && absolute_b <= high)
			{
				return (a + b) / 2;
			}

			if (absolute_a < low)
			{
				return (b / 2) + a;
			}

			if (absolute_b < low)
			{
				return (a / 2) + b;
			}

			return (a / 2) + (b / 2);
		}
	}

	template <typename T>
	[[nodiscard]] constexpr auto factorial(const T n) noexcept -> T
	{
		static_assert(std::is_integral_v<T>, "'T' is not an integral value!");

		if (n <= 1)
		{
			return 1;
		}

		T value = static_cast<T>(1);

		size_t counter = 0;
		while (counter < static_cast<size_t>(n))
		{
			value *= static_cast<T>(counter + 1);
			++counter;
		}

		return value;
	}

	template <typename T>
	[[nodiscard]] constexpr auto pow(const T x, const T n) noexcept -> T
	{
		static_assert(std::is_integral_v<T>, "'T' is not an integral value!");

		if (n <= 0)
		{
			return 1;
		}

		T value = static_cast<T>(1);

		size_t counter = 0;
		while (counter < static_cast<size_t>(n))
		{
			value *= x;
			++counter;
		}

		return value;
	}
} // namespace HyperEngine
