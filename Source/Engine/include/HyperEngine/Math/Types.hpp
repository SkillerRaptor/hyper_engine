/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <limits>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	[[nodiscard]] constexpr auto is_even(const T x) noexcept -> bool
	{
		static_assert(std::is_integral_v<T>, "'T' is not an integral value!");

		return (x & static_cast<T>(1)) == 0;
	}

	template <typename T>
	[[nodiscard]] constexpr auto is_odd(const T x) noexcept -> bool
	{
		static_assert(std::is_integral_v<T>, "'T' is not an integral value!");

		return (x & static_cast<T>(1)) != 0;
	}

	template <typename T>
	[[nodiscard]] constexpr auto is_nan(const T x) noexcept -> bool
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		return __builtin_isnan(x);
	}

	template <typename T>
	[[nodiscard]] constexpr auto is_positive_infinite(const T x) noexcept -> bool
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		return x == std::numeric_limits<T>::infinity();
	}

	template <typename T>
	[[nodiscard]] constexpr auto is_negative_infinite(const T x) noexcept -> bool
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		return x == -std::numeric_limits<T>::infinity();
	}

	template <typename T>
	[[nodiscard]] constexpr auto is_infinite(const T x) noexcept -> bool
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		return is_positive_infinite(x) || is_negative_infinite(x);
	}

	template <typename T>
	[[nodiscard]] constexpr auto is_finite(const T x) noexcept -> bool
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		return !is_nan(x) && !is_infinite(x);
	}
} // namespace HyperEngine
