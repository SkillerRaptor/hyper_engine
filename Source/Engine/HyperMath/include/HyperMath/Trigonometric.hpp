/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperMath/Constants.hpp"

#include <type_traits>

namespace HyperMath
{
	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	static constexpr auto radians(T&& degrees) -> T
	{
		return degrees * (Numbers::pi_v<T> / static_cast<T>(180.0L));
	}

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	static constexpr auto degrees(T&& radians) -> T
	{
		return radians * (static_cast<T>(180.0L) / Numbers::pi_v<T>);
	}

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	static constexpr auto lerp(T a, T b, T interpolation) -> T
	{
		if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0))
		{
			return interpolation * b + (1 - interpolation) * a;
		}

		if (interpolation == 1)
		{
			return b;
		}

		const T x = a + interpolation * (b - a);
		return (interpolation > 1) == (b > a) ? (b < x ? x : b) : (b > x ? x : b);
	}
} // namespace HyperMath
