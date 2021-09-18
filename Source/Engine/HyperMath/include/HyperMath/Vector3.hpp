/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Assertion.hpp>
#include <HyperCore/Compilers.hpp>

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <type_traits>

HYPERENGINE_DISABLE_WARNING_ANONYMOUS_STRUCT()

namespace HyperEngine
{
	template <class T>
	class Vector3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

	public:
		Vector3() = default;

		explicit Vector3(T scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
		{
		}

		Vector3(std::initializer_list<T> initializer_list)
		{
			HYPERENGINE_ASSERT(initializer_list.size() <= 3);

			std::uninitialized_copy(initializer_list.begin(), initializer_list.end(), data);
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
	using Vec3i = Vector3<int32_t>;
	using Vec3ui = Vector3<uint32_t>;
} // namespace HyperEngine

HYPERENGINE_RESTORE_WARNING_ANONYMOUS_STRUCT()
