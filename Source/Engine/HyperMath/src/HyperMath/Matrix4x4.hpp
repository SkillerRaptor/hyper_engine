/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace HyperMath
{
	template <typename T>
	class CMatrix4x4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for CMatrix4x4!");
	}

	using CMat4x4 = CMatrix4x4<float>;
	using CMat4x4f = CMatrix4x4<float>;
	using CMat4x4d = CMatrix4x4<double>;
	using CMat4x4i = CMatrix4x4<int32_t>;
	using CMat4x4ui = CMatrix4x4<uint32_t>;
} // namespace HyperMath
