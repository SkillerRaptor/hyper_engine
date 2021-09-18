/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace HyperEngine
{
	template <class T>
	class Matrix4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Mat4 = Matrix4<float>;
	using Mat4f = Matrix4<float>;
	using Mat4d = Matrix4<double>;
	using Mat4i = Matrix4<int32_t>;
	using Mat4ui = Matrix4<uint32_t>;
} // namespace HyperEngine
