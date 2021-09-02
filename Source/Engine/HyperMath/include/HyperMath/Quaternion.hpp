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
	class Quaternion
	{
	public:
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
	};

	using Quat = Quaternion<float>;
	using Quatf = Quaternion<float>;
	using Quatd = Quaternion<double>;
	using Quati = Quaternion<int>;
	using Quatui = Quaternion<unsigned int>;
} // namespace HyperMath
