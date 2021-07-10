/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
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
		
		CVector2<T> operator-() const
		{
			CVector2<T> vector;
			vector.x = -x;
			vector.y = -y;
			return vector;
		}
		
		CVector2<T> operator+(T value) const
		{
			CVector2<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			return vector;
		}
		
		CVector2<T> operator+(const CVector2<T>& other) const
		{
			CVector2<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			return vector;
		}
		
		CVector2<T>& operator+=(T value)
		{
			x += value;
			y += value;
			return *this;
		}
		
		CVector2<T>& operator+=(const CVector2<T>& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		
		CVector2<T> operator-(T value) const
		{
			CVector2<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			return vector;
		}
		
		CVector2<T> operator-(const CVector2<T>& other) const
		{
			CVector2<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			return vector;
		}
		
		CVector2<T>& operator-=(T value)
		{
			x -= value;
			y -= value;
			return *this;
		}
		
		CVector2<T>& operator-=(const CVector2<T>& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		
		CVector2<T> operator*(T value) const
		{
			CVector2<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			return vector;
		}
		
		CVector2<T> operator*(const CVector2<T>& other) const
		{
			CVector2<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			return vector;
		}
		
		CVector2<T>& operator*=(T value)
		{
			x *= value;
			y *= value;
			return *this;
		}
		
		CVector2<T>& operator*=(const CVector2<T>& other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}
		
		CVector2<T> operator/(T value) const
		{
			CVector2<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			return vector;
		}
		
		CVector2<T> operator/(const CVector2<T>& other) const
		{
			CVector2<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			return vector;
		}
		
		CVector2<T>& operator/=(T value)
		{
			x /= value;
			y /= value;
			return *this;
		}
		
		CVector2<T>& operator/=(const CVector2<T>& other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
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
} // namespace HyperMath
