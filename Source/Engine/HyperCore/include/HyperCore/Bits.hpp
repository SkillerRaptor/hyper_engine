/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperCore::Bits
{
	template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
	static constexpr auto bit(T shift) noexcept -> T
	{
		using U = std::make_unsigned_t<T>;
		return static_cast<U>(1) << shift;
	}

	template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
	static constexpr auto mask(T shift) noexcept -> T
	{
		using U = std::make_unsigned_t<T>;
		return bit(shift) - static_cast<U>(1);
	}
} // namespace HyperCore::Bits
