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
	static constexpr T bit(T shift)
	{
		return static_cast<T>(1) << shift;
	}

	template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
	static constexpr T mask(T shift)
	{
		return bit<T>(shift) - static_cast<T>(1);
	}
} // namespace HyperCore
