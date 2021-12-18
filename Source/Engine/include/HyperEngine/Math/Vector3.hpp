/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Compiler.hpp"

#include <cstdint>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	requires std::is_arithmetic_v<T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;
	};

	using Vec3 = Vector3<float>;
	using Vec3f = Vector3<float>;
	using Vec3d = Vector3<double>;
	using Vec3i = Vector3<int32_t>;
	using Vec3ui = Vector3<uint32_t>;
} // namespace HyperEngine
