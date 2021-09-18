/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperEngine
{
	template <class T>
	constexpr auto bit(const T shift) noexcept -> T
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		if (shift == 0)
		{
			return static_cast<T>(1);
		}

		using UnsignedT = typename std::make_unsigned_t<T>;
		
		const UnsignedT value = static_cast<UnsignedT>(1) << static_cast<UnsignedT>(shift);
		return static_cast<T>(value);
	}

	template <class T>
	constexpr auto mask(const T shift) noexcept -> T
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		if (shift == 0)
		{
			return static_cast<T>(1);
		}

		using UnsignedT = typename std::make_unsigned_t<T>;
		
		const UnsignedT value = static_cast<UnsignedT>(bit(shift)) << static_cast<UnsignedT>(1);
		return static_cast<T>(value);
	}
} // namespace HyperEngine
