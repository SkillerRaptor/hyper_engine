/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Compilers.hpp>

#include <type_traits>

HYPERENGINE_COMPILER_PUSH_WARNING
HYPERENGINE_COMPILER_PUSH_ANONYMOUS_STRUCT

namespace HyperMath
{
	template <typename T>
	class Vector4
	{
	public:
		static_assert(std::is_arithmetic<T>::value, "'T' is not an arithmetic value!");
		
	public:
		Vector4() = default;

		explicit Vector4(T scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
			, w(scalar)
		{
		}

		Vector4(T x_value, T y_value, T z_value, T w_value)
			: x(x_value)
			, y(y_value)
			, z(z_value)
			, w(w_value)
		{
		}

		auto operator-() const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = -x;
			vector.y = -y;
			vector.z = -z;
			vector.w = -w;
			return vector;
		}

		auto operator+(T value) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			vector.z = z + value;
			vector.w = w + value;
			return vector;
		}

		auto operator+(const Vector4<T>& other) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			vector.z = z + other.z;
			vector.w = w + other.w;
			return vector;
		}

		auto operator+=(T value) -> Vector4<T>&
		{
			x += value;
			y += value;
			z += value;
			w += value;
			return *this;
		}

		auto operator+=(const Vector4<T>& other) -> Vector4<T>&
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		auto operator-(T value) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			vector.z = z - value;
			vector.w = w - value;
			return vector;
		}

		auto operator-(const Vector4<T>& other) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			vector.z = z - other.z;
			vector.w = w - other.w;
			return vector;
		}

		auto operator-=(T value) -> Vector4<T>&
		{
			x -= value;
			y -= value;
			x -= value;
			y -= value;
			return *this;
		}

		auto operator-=(const Vector4<T>& other) -> Vector4<T>&
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		auto operator*(T value) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			vector.z = z * value;
			vector.w = w * value;
			return vector;
		}

		auto operator*(const Vector4<T>& other) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			vector.z = z * other.z;
			vector.w = w * other.w;
			return vector;
		}

		auto operator*=(T value) -> Vector4<T>&
		{
			x *= value;
			y *= value;
			z *= value;
			w *= value;
			return *this;
		}

		auto operator*=(const Vector4<T>& other) -> Vector4<T>&
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;
			return *this;
		}

		auto operator/(T value) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			vector.z = z / value;
			vector.w = w / value;
			return vector;
		}

		auto operator/(const Vector4<T>& other) const -> Vector4<T>
		{
			Vector4<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			vector.z = z / other.z;
			vector.w = w / other.w;
			return vector;
		}

		auto operator/=(T value) -> Vector4<T>&
		{
			x /= value;
			y /= value;
			z /= value;
			w /= value;
			return *this;
		}

		auto operator/=(const Vector4<T>& other) -> Vector4<T>&
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
			T data[4]{ 0 };

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

	using Vec4 = Vector4<float>;
	using Vec4f = Vector4<float>;
	using Vec4d = Vector4<double>;
	using Vec4i = Vector4<int>;
	using Vec4ui = Vector4<unsigned int>;
} // namespace HyperMath

HYPERENGINE_COMPILER_POP_WARNING
