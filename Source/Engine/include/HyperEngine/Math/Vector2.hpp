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
	class Vector2
	{
	public:
		HYPERENGINE_DISABLE_WARNINGS()
		union
		{
			T data[2] = { 0 };

			struct
			{
				T x;
				T y;
			};

			struct
			{
				T s;
				T t;
			};

			struct
			{
				T u;
				T v;
			};
		};
		HYPERENGINE_RESTORE_WARNINGS()
	};

	using Vec2 = Vector2<float>;
	using Vec2f = Vector2<float>;
	using Vec2d = Vector2<double>;
	using Vec2i = Vector2<int32_t>;
	using Vec2ui = Vector2<uint32_t>;
} // namespace HyperEngine
