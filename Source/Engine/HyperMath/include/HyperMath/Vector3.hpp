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
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	class Vector3
	{
	public:
		Vector3() = default;

		explicit Vector3(T scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
		{
		}

		Vector3(T x_value, T y_value, T z_value)
			: x(x_value)
			, y(y_value)
			, z(z_value)
		{
		}

		auto operator-() const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = -x;
			vector.y = -y;
			vector.z = -z;
			return vector;
		}

		auto operator+(T value) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			vector.z = z + value;
			return vector;
		}

		auto operator+(const Vector3<T>& other) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			vector.z = z + other.z;
			return vector;
		}

		auto operator+=(T value) -> Vector3<T>&
		{
			x += value;
			y += value;
			z += value;
			return *this;
		}

		auto operator+=(const Vector3<T>& other) -> Vector3<T>&
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		auto operator-(T value) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			vector.z = z - value;
			return vector;
		}

		auto operator-(const Vector3<T>& other) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			vector.z = z - other.z;
			return vector;
		}

		auto operator-=(T value) -> Vector3<T>&
		{
			x -= value;
			y -= value;
			return *this;
		}

		auto operator-=(const Vector3<T>& other) -> Vector3<T>&
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		auto operator*(T value) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			vector.z = z * value;
			return vector;
		}

		auto operator*(const Vector3<T>& other) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			vector.z = z * other.z;
			return vector;
		}

		auto operator*=(T value) -> Vector3<T>&
		{
			x *= value;
			y *= value;
			z *= value;
			return *this;
		}

		auto operator*=(const Vector3<T>& other) -> Vector3<T>&
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		auto operator/(T value) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			vector.z = z / value;
			return vector;
		}

		auto operator/(const Vector3<T>& other) const -> Vector3<T>
		{
			Vector3<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			vector.z = z / other.z;
			return vector;
		}

		auto operator/=(T value) -> Vector3<T>&
		{
			x /= value;
			y /= value;
			z /= value;
			return *this;
		}

		auto operator/=(const Vector3<T>& other) -> Vector3<T>&
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			return *this;
		}

	public:
		union
		{
			T data[3]{ 0 };

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

	using Vec3 = Vector3<float>;
	using Vec3f = Vector3<float>;
	using Vec3d = Vector3<double>;
	using Vec3i = Vector3<int>;
	using Vec3ui = Vector3<unsigned int>;
} // namespace HyperMath

HYPERENGINE_COMPILER_POP_WARNING
