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
	class CVector3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for CVector3!");

	public:
		CVector3()
			: x(static_cast<T>(0))
			, y(static_cast<T>(0))
			, z(static_cast<T>(0))
		{
		}

		explicit CVector3(T scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
		{
		}

		CVector3(T x, T y, T z)
			: x(x)
			, y(y)
			, z(z)
		{
		}

	public:
		union
		{
			T data[3];

			struct
			{
				T x;
				T y;
				T z;
			};

			struct
			{
				T r;
				T g;
				T b;
			};

			struct
			{
				T s;
				T t;
				T p;
			};

			struct
			{
				T u;
				T v;
				T w;
			};
		};
	};

	using CVec3 = CVector3<float>;
	using CVec3f = CVector3<float>;
	using CVec3d = CVector3<double>;
	using CVec3i = CVector3<int32_t>;
	using CVec3ui = CVector3<uint32_t>;
} // namespace HyperMath
