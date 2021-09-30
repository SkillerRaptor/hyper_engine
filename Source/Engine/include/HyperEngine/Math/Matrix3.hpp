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
	class CMatrix3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Mat3 = CMatrix3<float>;
	using Mat3f = CMatrix3<float>;
	using Mat3d = CMatrix3<double>;
	using Mat3i = CMatrix3<int32_t>;
	using Mat3ui = CMatrix3<uint32_t>;
} // namespace HyperEngine
