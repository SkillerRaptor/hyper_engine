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
	class CQuaternion
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");
	};

	using Quat = CQuaternion<float>;
	using Quatf = CQuaternion<float>;
	using Quatd = CQuaternion<double>;
	using Quati = CQuaternion<int32_t>;
	using Quatui = CQuaternion<uint32_t>;
} // namespace HyperEngine
