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
	static constexpr bool equal(T a, U b)
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
	static constexpr bool not_equal(T a, U b)
	{
		return !equal(a, b);
	}

	template <typename T, typename U>
	static constexpr bool less(T a, U b)
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
	static constexpr bool greater(T a, U b)
	{
		return cmp_less(b, a);
	}

	template <typename T, typename U>
	static constexpr bool less_equal(T a, U b)
	{
		return !greater(a, b);
	}

	template <typename T, typename U>
	static constexpr bool greater_equal(T a, U b)
	{
		return !less(a, b);
	}
} // namespace HyperMath::Compare
