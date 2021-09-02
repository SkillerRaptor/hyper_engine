/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T>
	class Matrix3x3
	{
	public:
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
	};

	using Mat3x3 = Matrix3x3<float>;
	using Mat3x3f = Matrix3x3<float>;
	using Mat3x3d = Matrix3x3<double>;
	using Mat3x3i = Matrix3x3<int>;
	using Mat3x3ui = Matrix3x3<unsigned int>;
} // namespace HyperMath
