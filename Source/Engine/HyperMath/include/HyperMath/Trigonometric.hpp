/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperMath/Constants.hpp"

#include <limits>
#include <type_traits>

namespace HyperEngine
{
	template <class T>
	constexpr auto radians(const T degrees) noexcept -> T
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		return degrees * (Constants::pi_v<T> / static_cast<T>(180.0L));
	}

	template <class T>
	constexpr auto degrees(const T radians) noexcept -> T
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		return radians * (static_cast<T>(180.0L) / Constants::pi_v<T>);
	}

	template <class T>
	constexpr auto lerp(const T a, const T b, const T interpolation) noexcept -> T
	{
		static_assert(std::is_floating_point_v<T>, "'T' is not a floating point value!");

		if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0))
		{
			return interpolation * b + (1 - interpolation) * a;
		}

		if (interpolation == 1)
		{
			return b;
		}

		const bool interpolated = (interpolation > 1) == (b > a);
		const T value = a + interpolation * (b - a);
		const T first_value = (b < value) ? value : b;
		const T second_value = (b > value) ? value : b;
		return interpolated ? first_value : second_value;
	}
} // namespace HyperEngine
