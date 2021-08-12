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
}
