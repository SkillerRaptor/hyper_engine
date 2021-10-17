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
	class CVector2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "'T' is not an arithmetic value!");

	public:
		CVector2() = default;

		explicit CVector2(const T t_scalar)
			: x(t_scalar)
			, y(t_scalar)
		{
		}

		CVector2(const T t_x, const T t_y)
			: x(t_x)
			, y(t_y)
		{
		}

		auto operator-() const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = -x;
			vector.y = -y;
			return vector;
		}

		auto operator+(const T value) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			return vector;
		}

		auto operator+(const CVector2<T>& other) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			return vector;
		}

		auto operator+=(const T value) -> CVector2<T>&
		{
			x += value;
			y += value;
			return *this;
		}

		auto operator+=(const CVector2<T>& other) -> CVector2<T>&
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		auto operator-(const T value) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			return vector;
		}

		auto operator-(const CVector2<T>& other) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			return vector;
		}

		auto operator-=(const T value) -> CVector2<T>&
		{
			x -= value;
			y -= value;
			return *this;
		}

		auto operator-=(const CVector2<T>& other) -> CVector2<T>&
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		auto operator*(const T value) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			return vector;
		}

		auto operator*(const CVector2<T>& other) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			return vector;
		}

		auto operator*=(const T value) -> CVector2<T>&
		{
			x *= value;
			y *= value;
			return *this;
		}

		auto operator*=(const CVector2<T>& other) -> CVector2<T>&
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		auto operator/(const T value) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			return vector;
		}

		auto operator/(const CVector2<T>& other) const -> CVector2<T>
		{
			CVector2<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			return vector;
		}

		auto operator/=(const T value) -> CVector2<T>&
		{
			x /= value;
			y /= value;
			return *this;
		}

		auto operator/=(const CVector2<T>& other) -> CVector2<T>&
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}

	public:
		union
		{
			T data[2] = { 0 };

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

	using Vec2 = CVector2<float>;
	using Vec2f = CVector2<float>;
	using Vec2d = CVector2<double>;
	using Vec2i = CVector2<int32_t>;
	using Vec2ui = CVector2<uint32_t>;
} // namespace HyperEngine

HYPERENGINE_RESTORE_WARNING_ANONYMOUS_STRUCT()
