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
	class CMatrix2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Mat2 = CMatrix2<float>;
	using Mat2f = CMatrix2<float>;
	using Mat2d = CMatrix2<double>;
	using Mat2i = CMatrix2<int32_t>;
	using Mat2ui = CMatrix2<uint32_t>;
} // namespace HyperEngine
