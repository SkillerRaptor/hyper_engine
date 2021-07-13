/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperCore
{
	template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
	static constexpr T Bit(T shift) noexcept
	{
		return static_cast<T>(1) << shift;
	}
	
	template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
	static constexpr T Mask(T shift) noexcept
	{
		return Bit<T>(shift) - static_cast<T>(1);
	}
} // namespace HyperCore
