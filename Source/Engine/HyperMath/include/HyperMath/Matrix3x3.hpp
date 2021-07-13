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
	template <typename T>
	class CMatrix3x3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for CMatrix3x3!");
	};

	using CMat3x3 = CMatrix3x3<float>;
	using CMat3x3f = CMatrix3x3<float>;
	using CMat3x3d = CMatrix3x3<double>;
	using CMat3x3i = CMatrix3x3<int32_t>;
	using CMat3x3ui = CMatrix3x3<uint32_t>;
} // namespace HyperMath
