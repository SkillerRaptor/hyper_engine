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
	static constexpr T radians(T&& degrees)
	{
		return degrees * (g_PI / 180.0f);
	}
	
	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	static constexpr T degrees(T&& radians)
	{
		return radians * (180.0f / g_PI);
	}
}
