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
	class CVector4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

	public:
		CVector4() = default;

		explicit CVector4(const T t_scalar)
			: x(t_scalar)
			, y(t_scalar)
			, z(t_scalar)
			, w(t_scalar)
		{
		}

		CVector4(const T t_x, const T t_y, const T t_z, const T t_w)
			: x(t_x)
			, y(t_y)
			, z(t_z)
			, w(t_w)
		{
		}

		auto operator-() const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = -x;
			vector.y = -y;
			vector.z = -z;
			vector.w = -w;
			return vector;
		}

		auto operator+(const T value) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			vector.z = z + value;
			vector.w = w + value;
			return vector;
		}

		auto operator+(const CVector4<T>& other) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			vector.z = z + other.z;
			vector.w = w + other.w;
			return vector;
		}

		auto operator+=(const T value) -> CVector4<T>&
		{
			x += value;
			y += value;
			z += value;
			w += value;
			return *this;
		}

		auto operator+=(const CVector4<T>& other) -> CVector4<T>&
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		auto operator-(const T value) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			vector.z = z - value;
			vector.w = w - value;
			return vector;
		}

		auto operator-(const CVector4<T>& other) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			vector.z = z - other.z;
			vector.w = w - other.w;
			return vector;
		}

		auto operator-=(const T value) -> CVector4<T>&
		{
			x -= value;
			y -= value;
			x -= value;
			y -= value;
			return *this;
		}

		auto operator-=(const CVector4<T>& other) -> CVector4<T>&
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		auto operator*(const T value) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			vector.z = z * value;
			vector.w = w * value;
			return vector;
		}

		auto operator*(const CVector4<T>& other) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			vector.z = z * other.z;
			vector.w = w * other.w;
			return vector;
		}

		auto operator*=(const T value) -> CVector4<T>&
		{
			x *= value;
			y *= value;
			z *= value;
			w *= value;
			return *this;
		}

		auto operator*=(const CVector4<T>& other) -> CVector4<T>&
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;
			return *this;
		}

		auto operator/(const T value) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			vector.z = z / value;
			vector.w = w / value;
			return vector;
		}

		auto operator/(const CVector4<T>& other) const -> CVector4<T>
		{
			CVector4<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			vector.z = z / other.z;
			vector.w = w / other.w;
			return vector;
		}

		auto operator/=(const T value) -> CVector4<T>&
		{
			x /= value;
			y /= value;
			z /= value;
			w /= value;
			return *this;
		}

		auto operator/=(const CVector4<T>& other) -> CVector4<T>&
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
			T data[4] = { 0 };

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

	using Vec4 = CVector4<float>;
	using Vec4f = CVector4<float>;
	using Vec4d = CVector4<double>;
	using Vec4i = CVector4<int32_t>;
	using Vec4ui = CVector4<uint32_t>;
} // namespace HyperEngine

HYPERENGINE_RESTORE_WARNING_ANONYMOUS_STRUCT()
