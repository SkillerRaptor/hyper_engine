/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	class Quaternion
	{
	public:
	};

	using Quat = Quaternion<float>;
	using Quatf = Quaternion<float>;
	using Quatd = Quaternion<double>;
	using Quati = Quaternion<int>;
	using Quatui = Quaternion<unsigned int>;
} // namespace HyperMath
