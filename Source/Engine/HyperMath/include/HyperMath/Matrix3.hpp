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
	class Matrix3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Mat3 = Matrix3<float>;
	using Mat3f = Matrix3<float>;
	using Mat3d = Matrix3<double>;
	using Mat3i = Matrix3<int32_t>;
	using Mat3ui = Matrix3<uint32_t>;
} // namespace HyperEngine
