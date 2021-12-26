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
	class Vector2
	{
	public:
		T x = 0;
		T y = 0;
	};

	using Vec2 = Vector2<float>;
	using Vec2f = Vector2<float>;
	using Vec2d = Vector2<double>;
	using Vec2i = Vector2<int32_t>;
	using Vec2ui = Vector2<uint32_t>;
} // namespace HyperEngine
