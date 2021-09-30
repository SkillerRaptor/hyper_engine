/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Assertion.hpp"
#include "HyperEngine/Core/Compilers.hpp"

#include <cstdint>
#include <type_traits>

HYPERENGINE_DISABLE_WARNING_ANONYMOUS_STRUCT()

namespace HyperEngine
{
	template <typename T>
	class CVector3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

	public:
		CVector3() = default;

		explicit CVector3(T t_scalar)
			: x(t_scalar)
			, y(t_scalar)
			, z(t_scalar)
		{
		}

		CVector3(T t_x, T t_y, T t_z)
			: x(t_x)
			, y(t_y)
			, z(t_z)
		{
		}

		auto operator-() const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = -x;
			vector.y = -y;
			vector.z = -z;
			return vector;
		}

		auto operator+(T value) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			vector.z = z + value;
			return vector;
		}

		auto operator+(const CVector3<T>& other) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			vector.z = z + other.z;
			return vector;
		}

		auto operator+=(T value) -> CVector3<T>&
		{
			x += value;
			y += value;
			z += value;
			return *this;
		}

		auto operator+=(const CVector3<T>& other) -> CVector3<T>&
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		auto operator-(T value) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			vector.z = z - value;
			return vector;
		}

		auto operator-(const CVector3<T>& other) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			vector.z = z - other.z;
			return vector;
		}

		auto operator-=(T value) -> CVector3<T>&
		{
			x -= value;
			y -= value;
			return *this;
		}

		auto operator-=(const CVector3<T>& other) -> CVector3<T>&
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		auto operator*(T value) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			vector.z = z * value;
			return vector;
		}

		auto operator*(const CVector3<T>& other) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			vector.z = z * other.z;
			return vector;
		}

		auto operator*=(T value) -> CVector3<T>&
		{
			x *= value;
			y *= value;
			z *= value;
			return *this;
		}

		auto operator*=(const CVector3<T>& other) -> CVector3<T>&
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		auto operator/(T value) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			vector.z = z / value;
			return vector;
		}

		auto operator/(const CVector3<T>& other) const -> CVector3<T>
		{
			CVector3<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			vector.z = z / other.z;
			return vector;
		}

		auto operator/=(T value) -> CVector3<T>&
		{
			x /= value;
			y /= value;
			z /= value;
			return *this;
		}

		auto operator/=(const CVector3<T>& other) -> CVector3<T>&
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

	using Vec3 = CVector3<float>;
	using Vec3f = CVector3<float>;
	using Vec3d = CVector3<double>;
	using Vec3i = CVector3<int32_t>;
	using Vec3ui = CVector3<uint32_t>;
} // namespace HyperEngine

HYPERENGINE_RESTORE_WARNING_ANONYMOUS_STRUCT()
