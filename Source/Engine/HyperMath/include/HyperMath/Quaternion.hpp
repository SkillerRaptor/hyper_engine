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
	class CQuaternion
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for CQuaternion!");
	};

	using CQuat = CQuaternion<float>;
	using CQuatf = CQuaternion<float>;
	using CQuatd = CQuaternion<double>;
	using CQuati = CQuaternion<int32_t>;
	using CQuatui = CQuaternion<uint32_t>;
} // namespace HyperMath
