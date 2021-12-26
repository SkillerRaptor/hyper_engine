/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Compiler.hpp"
#include "HyperEngine/Support/Concepts.hpp"

#include <cstdint>

namespace HyperEngine
{
	template <Arithmetic T>
	class Vector3
	{
	public:
		T x = 0;
		T y = 0;
		T z = 0;
	};

	using Vec3 = Vector3<float>;
	using Vec3f = Vector3<float>;
	using Vec3d = Vector3<double>;
	using Vec3i = Vector3<int32_t>;
	using Vec3ui = Vector3<uint32_t>;
} // namespace HyperEngine
