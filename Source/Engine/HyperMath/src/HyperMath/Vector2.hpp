/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T>
	class CVector2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for CVector2!");
	
	public:
		CVector2()
			: x(static_cast<T>(0))
			, y(static_cast<T>(0))
		{
		}
		
		explicit CVector2(T scalar)
			: x(scalar)
			, y(scalar)
		{
		}
		
		CVector2(T x, T y)
			: x(x)
			, y(y)
		{
		}
	
	public:
		union
		{
			T data[2];
			
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
	};
	
	using CVec2 = CVector2<float>;
	using CVec2f = CVector2<float>;
	using CVec2d = CVector2<double>;
	using CVec2i = CVector2<int32_t>;
	using CVec2ui = CVector2<uint32_t>;
}
