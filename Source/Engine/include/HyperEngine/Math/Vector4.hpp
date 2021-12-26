/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Compiler.hpp"
#include "HyperEngine/Support/Concepts.hpp"

#include <cstdint>

namespace HyperEngine
{
	template <Arithmetic T>
	class Vector4
	{
	public:
		T x = 0;
		T y = 0;
		T z = 0;
		T w = 0;
	};

	using Vec4 = Vector4<float>;
	using Vec4f = Vector4<float>;
	using Vec4d = Vector4<double>;
	using Vec4i = Vector4<int32_t>;
	using Vec4ui = Vector4<uint32_t>;
} // namespace HyperEngine
