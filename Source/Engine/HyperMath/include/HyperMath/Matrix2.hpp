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
	class Matrix2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Mat2 = Matrix2<float>;
	using Mat2f = Matrix2<float>;
	using Mat2d = Matrix2<double>;
	using Mat2i = Matrix2<int32_t>;
	using Mat2ui = Matrix2<uint32_t>;
} // namespace HyperEngine
