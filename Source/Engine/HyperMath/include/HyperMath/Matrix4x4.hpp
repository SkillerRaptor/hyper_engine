/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace HyperMath
{
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	class Matrix4x4
	{
	public:
	};

	using Mat4x4 = Matrix4x4<float>;
	using Mat4x4f = Matrix4x4<float>;
	using Mat4x4d = Matrix4x4<double>;
	using Mat4x4i = Matrix4x4<int>;
	using Mat4x4ui = Matrix4x4<unsigned int>;
} // namespace HyperMath
