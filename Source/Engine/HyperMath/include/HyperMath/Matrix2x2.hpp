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
	class CMatrix2x2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for CMatrix2x2!");
	};

	using CMat2x2 = CMatrix2x2<float>;
	using CMat2x2f = CMatrix2x2<float>;
	using CMat2x2d = CMatrix2x2<double>;
	using CMat2x2i = CMatrix2x2<int32_t>;
	using CMat2x2ui = CMatrix2x2<uint32_t>;
} // namespace HyperMath
