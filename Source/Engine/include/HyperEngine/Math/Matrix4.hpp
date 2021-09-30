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
	template <typename T>
	class CMatrix4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Mat4 = CMatrix4<float>;
	using Mat4f = CMatrix4<float>;
	using Mat4d = CMatrix4<double>;
	using Mat4i = CMatrix4<int32_t>;
	using Mat4ui = CMatrix4<uint32_t>;
} // namespace HyperEngine
