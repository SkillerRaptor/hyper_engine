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
	class CVector4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for CVector4!");

	public:
		CVector4()
			: x(static_cast<T>(0))
			, y(static_cast<T>(0))
			, z(static_cast<T>(0))
			, w(static_cast<T>(0))
		{
		}

		explicit CVector4(T scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
			, w(scalar)
		{
		}

		CVector4(T x, T y, T z, T w)
			: x(x)
			, y(y)
			, z(z)
			, w(w)
		{
		}
		
		CVector4<T> operator-() const
		{
			CVector4<T> vector;
			vector.x = -x;
			vector.y = -y;
			vector.z = -z;
			vector.w = -w;
			return vector;
		}
		
		CVector4<T> operator+(T value) const
		{
			CVector4<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			vector.z = z + value;
			vector.w = w + value;
			return vector;
		}
		
		CVector4<T> operator+(const CVector4<T>& other) const
		{
			CVector4<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			vector.z = z + other.z;
			vector.w = w + other.w;
			return vector;
		}
		
		CVector4<T>& operator+=(T value)
		{
			x += value;
			y += value;
			z += value;
			w += value;
			return *this;
		}
		
		CVector4<T>& operator+=(const CVector4<T>& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}
		
		CVector4<T> operator-(T value) const
		{
			CVector4<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			vector.z = z - value;
			vector.w = w - value;
			return vector;
		}
		
		CVector4<T> operator-(const CVector4<T>& other) const
		{
			CVector4<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			vector.z = z - other.z;
			vector.w = w - other.w;
			return vector;
		}
		
		CVector4<T>& operator-=(T value)
		{
			x -= value;
			y -= value;
			x -= value;
			y -= value;
			return *this;
		}
		
		CVector4<T>& operator-=(const CVector4<T>& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}
		
		CVector4<T> operator*(T value) const
		{
			CVector4<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			vector.z = z * value;
			vector.w = w * value;
			return vector;
		}
		
		CVector4<T> operator*(const CVector4<T>& other) const
		{
			CVector4<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			vector.z = z * other.z;
			vector.w = w * other.w;
			return vector;
		}
		
		CVector4<T>& operator*=(T value)
		{
			x *= value;
			y *= value;
			z *= value;
			w *= value;
			return *this;
		}
		
		CVector4<T>& operator*=(const CVector4<T>& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;
			return *this;
		}
		
		CVector4<T> operator/(T value) const
		{
			CVector4<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			vector.z = z / value;
			vector.w = w / value;
			return vector;
		}
		
		CVector4<T> operator/(const CVector4<T>& other) const
		{
			CVector4<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			vector.z = z / other.z;
			vector.w = w / other.w;
			return vector;
		}
		
		CVector4<T>& operator/=(T value)
		{
			x /= value;
			y /= value;
			z /= value;
			w /= value;
			return *this;
		}
		
		CVector4<T>& operator/=(const CVector4<T>& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			w /= other.w;
			return *this;
		}

	public:
		union
		{
			T data[4];

			struct
			{
				T x;
				T y;
				T z;
				T w;
			};

			struct
			{
				T r;
				T g;
				T b;
				T a;
			};

			struct
			{
				T s;
				T t;
				T p;
				T q;
			};
		};
	};

	using CVec4 = CVector4<float>;
	using CVec4f = CVector4<float>;
	using CVec4d = CVector4<double>;
	using CVec4i = CVector4<int32_t>;
	using CVec4ui = CVector4<uint32_t>;
} // namespace HyperMath
