/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <limits>
#include <type_traits>

namespace HyperMath
{
	template <typename T>
	constexpr auto is_even(T x) noexcept -> bool
	{
		static_assert(std::is_integral<T>::value, "'T' is not an integral value!");
		return (x & static_cast<T>(1)) == 0;
	}

	template <typename T>
	constexpr auto is_odd(T x) noexcept -> bool
	{
		static_assert(std::is_integral<T>::value, "'T' is not an integral value!");
		return (x & static_cast<T>(1)) != 0;
	}

	template <typename T>
	constexpr auto is_nan(T x) noexcept -> bool
	{
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
		return x != x;
	}

	template <typename T>
	constexpr auto is_positive_infinite(T x) noexcept -> bool
	{
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
		return x == std::numeric_limits<T>::infinity();
	}

	template <typename T>
	constexpr auto is_negative_infinite(T x) noexcept -> bool
	{
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
		return x == -std::numeric_limits<T>::infinity();
	}

	template <typename T>
	constexpr auto is_infinite(T x) noexcept -> bool
	{
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
		return is_positive_infinite(x) || is_negative_infinite(x);
	}

	template <typename T>
	constexpr auto is_finite(T x) noexcept -> bool
	{
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
		return (!is_nan(x)) && (!is_infinite(x));
	}
} // namespace HyperMath
