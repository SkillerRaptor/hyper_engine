/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperMath/Common.hpp"
#include "HyperMath/Constants.hpp"
#include "HyperMath/Types.hpp"

#include <limits>
#include <type_traits>

namespace HyperMath
{
	template <typename T>
	constexpr auto radians(T&& degrees) noexcept -> T
	{
		static_assert(std::is_floating_point<T>::value, "'T' is not a floating value!");
		return degrees * (Numbers::g_pi_v<T> / static_cast<T>(180.0L));
	}

	template <typename T>
	constexpr auto degrees(T&& radians) noexcept -> T
	{
		static_assert(std::is_floating_point<T>::value, "'T' is not a floating value!");
		return radians * (static_cast<T>(180.0L) / Numbers::g_pi_v<T>);
	}

	template <typename T>
	constexpr auto lerp(T a, T b, T interpolation) noexcept -> T
	{
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
		
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
