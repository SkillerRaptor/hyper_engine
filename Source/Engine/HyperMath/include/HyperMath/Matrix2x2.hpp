/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	class Matrix2x2
	{
	public:
	};

	using Mat2x2 = Matrix2x2<float>;
	using Mat2x2f = Matrix2x2<float>;
	using Mat2x2d = Matrix2x2<double>;
	using Mat2x2i = Matrix2x2<int>;
	using Mat2x2ui = Matrix2x2<unsigned int>;
} // namespace HyperMath
