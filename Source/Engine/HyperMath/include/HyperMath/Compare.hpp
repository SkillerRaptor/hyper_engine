/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperMath::Compare
{
	template <typename T, typename U>
	static constexpr auto equal(T a, U b) -> bool
	{
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

	template <typename T, typename U>
	static constexpr auto not_equal(T a, U b) -> bool
	{
		return !equal(a, b);
	}

	template <typename T, typename U>
	static constexpr auto less(T a, U b) -> bool
	{
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

	template <typename T, typename U>
	static constexpr auto greater(T a, U b) -> bool
	{
		return cmp_less(b, a);
	}

	template <typename T, typename U>
	static constexpr auto less_equal(T a, U b) -> bool
	{
		return !greater(a, b);
	}

	template <typename T, typename U>
	static constexpr auto greater_equal(T a, U b) -> bool
	{
		return !less(a, b);
	}
} // namespace HyperMath::Compare
