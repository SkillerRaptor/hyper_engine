/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperEngine
{
	template <class T, class U>
	constexpr auto equal(const T a, const U b) noexcept -> bool
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
		{
			return a == b;
		}
		else if constexpr (std::is_signed_v<T>)
		{
			return a >= 0 && std::make_unsigned_t<T>(a) == b;
		}
		else
		{
			return b >= 0 && a == std::make_unsigned_t<U>(b);
		}
	}

	template <class T, class U>
	constexpr auto not_equal(const T a, const U b) noexcept -> bool
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return !equal(a, b);
	}

	template <class T, class U>
	constexpr auto less(const T a, const U b) noexcept -> bool
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
		{
			return a < b;
		}
		else if constexpr (std::is_signed_v<T>)
		{
			return a < 0 || std::make_unsigned_t<T>(a) < b;
		}
		else
		{
			return b >= 0 && a < std::make_unsigned_t<U>(b);
		}
	}

	template <class T, class U>
	constexpr auto greater(const T a, const U b) noexcept -> bool
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return less(b, a);
	}

	template <class T, class U>
	constexpr auto less_equal(const T a, const U b) noexcept -> bool
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return !greater(a, b);
	}

	template <class T, class U>
	constexpr auto greater_equal(const T a, const U b) noexcept -> bool
	{
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

		return !less(a, b);
	}
} // namespace HyperEngine
