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
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	static constexpr auto is_even(T x) -> bool
	{
		return (x & static_cast<T>(1)) == 0;
	}

	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	static constexpr auto is_odd(T x) -> bool
	{
		return (x & static_cast<T>(1)) != 0;
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto is_nan(T x) -> bool
	{
		return x != x;
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto is_positive_infinite(T x) -> bool
	{
		return x == std::numeric_limits<T>::infinity();
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto is_negative_infinite(T x) -> bool
	{
		return x == -std::numeric_limits<T>::infinity();
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto is_infinite(T x) -> bool
	{
		return is_positive_infinite(x) || is_negative_infinite(x);
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static constexpr auto is_finite(T x) -> bool
	{
		return (!is_nan(x)) && (!is_infinite(x));
	}
} // namespace HyperMath
