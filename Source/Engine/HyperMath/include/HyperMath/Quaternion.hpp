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
	class Quaternion
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Quat = Quaternion<float>;
	using Quatf = Quaternion<float>;
	using Quatd = Quaternion<double>;
	using Quati = Quaternion<int32_t>;
	using Quatui = Quaternion<uint32_t>;
} // namespace HyperEngine
