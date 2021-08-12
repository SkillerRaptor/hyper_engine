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
	static constexpr auto bit(T shift) -> T
	{
		return static_cast<T>(1) << shift;
	}

	template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
	static constexpr auto mask(T shift) -> T
	{
		return bit<T>(shift) - static_cast<T>(1);
	}
} // namespace HyperCore
