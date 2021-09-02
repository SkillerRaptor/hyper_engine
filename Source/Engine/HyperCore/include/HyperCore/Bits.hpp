/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperCore
{
	template <typename T>
	constexpr auto bit(T shift) noexcept -> T
	{
		static_assert(std::is_integral<T>::value, "'T' is not an integral value!");
		static_assert(std::is_unsigned<T>::value, "'T' is not an unsigned value!");

		if (shift <= 0)
		{
			return 1;
		}

		using U = std::make_unsigned<T>::type;
		return static_cast<U>(1) << shift;
	}

	template <typename T>
	constexpr auto mask(T shift) noexcept -> T
	{
		static_assert(std::is_integral<T>::value, "'T' is not an integral value!");
		static_assert(std::is_unsigned<T>::value, "'T' is not an unsigned value!");

		if (shift <= 0)
		{
			return 1;
		}

		using U = std::make_unsigned<T>::type;
		return bit(shift) - static_cast<U>(1);
	}
} // namespace HyperCore
