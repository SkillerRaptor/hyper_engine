/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Compiler.hpp"

#include <cstdint>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	requires std::is_arithmetic_v<T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;
	};

	using Vec4 = Vector4<float>;
	using Vec4f = Vector4<float>;
	using Vec4d = Vector4<double>;
	using Vec4i = Vector4<int32_t>;
	using Vec4ui = Vector4<uint32_t>;
} // namespace HyperEngine
