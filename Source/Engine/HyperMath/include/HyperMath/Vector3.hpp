/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <type_traits>

#if HYPERENGINE_COMPILER_Clang
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif HYPERENGINE_COMPILER_GNU
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wpedantic"
#elif HYPERENGINE_COMPILER_MSVC
#	pragma warning(push)
#	pragma warning(disable : 4201)
#endif

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
		
		CVector3<T> operator-() const
		{
			CVector3<T> vector;
			vector.x = -x;
			vector.y = -y;
			vector.z = -z;
			return vector;
		}
		
		CVector3<T> operator+(T value) const
		{
			CVector3<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			vector.z = z + value;
			return vector;
		}
		
		CVector3<T> operator+(const CVector3<T>& other) const
		{
			CVector3<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			vector.z = z + other.z;
			return vector;
		}
		
		CVector3<T>& operator+=(T value)
		{
			x += value;
			y += value;
			z += value;
			return *this;
		}
		
		CVector3<T>& operator+=(const CVector3<T>& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}
		
		CVector3<T> operator-(T value) const
		{
			CVector3<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			vector.z = z - value;
			return vector;
		}
		
		CVector3<T> operator-(const CVector3<T>& other) const
		{
			CVector3<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			vector.z = z - other.z;
			return vector;
		}
		
		CVector3<T>& operator-=(T value)
		{
			x -= value;
			y -= value;
			return *this;
		}
		
		CVector3<T>& operator-=(const CVector3<T>& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}
		
		CVector3<T> operator*(T value) const
		{
			CVector3<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			vector.z = z * value;
			return vector;
		}
		
		CVector3<T> operator*(const CVector3<T>& other) const
		{
			CVector3<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			vector.z = z * other.z;
			return vector;
		}
		
		CVector3<T>& operator*=(T value)
		{
			x *= value;
			y *= value;
			z *= value;
			return *this;
		}
		
		CVector3<T>& operator*=(const CVector3<T>& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}
		
		CVector3<T> operator/(T value) const
		{
			CVector3<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			vector.z = z / value;
			return vector;
		}
		
		CVector3<T> operator/(const CVector3<T>& other) const
		{
			CVector3<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			vector.z = z / other.z;
			return vector;
		}
		
		CVector3<T>& operator/=(T value)
		{
			x /= value;
			y /= value;
			z /= value;
			return *this;
		}
		
		CVector3<T>& operator/=(const CVector3<T>& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			return *this;
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

#if HYPERENGINE_COMPILER_Clang
#	pragma clang diagnostic pop
#elif HYPERENGINE_COMPILER_GNU
#	pragma GCC diagnostic pop
#elif HYPERENGINE_COMPILER_MSVC
#	pragma warning(pop)
#endif
