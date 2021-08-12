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
	class Vector2
	{
	public:
		Vector2() = default;

		explicit Vector2(T scalar)
			: x(scalar)
			, y(scalar)
		{
		}

		Vector2(T x_value, T y_value)
			: x(x_value)
			, y(y_value)
		{
		}

		auto operator-() const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = -x;
			vector.y = -y;
			return vector;
		}

		auto operator+(T value) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x + value;
			vector.y = y + value;
			return vector;
		}

		auto operator+(const Vector2<T>& other) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x + other.x;
			vector.y = y + other.y;
			return vector;
		}

		auto operator+=(T value) -> Vector2<T>&
		{
			x += value;
			y += value;
			return *this;
		}

		auto operator+=(const Vector2<T>& other) -> Vector2<T>&
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		auto operator-(T value) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x - value;
			vector.y = y - value;
			return vector;
		}

		auto operator-(const Vector2<T>& other) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x - other.x;
			vector.y = y - other.y;
			return vector;
		}

		auto operator-=(T value) -> Vector2<T>&
		{
			x -= value;
			y -= value;
			return *this;
		}

		auto operator-=(const Vector2<T>& other) -> Vector2<T>&
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		auto operator*(T value) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x * value;
			vector.y = y * value;
			return vector;
		}

		auto operator*(const Vector2<T>& other) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x * other.x;
			vector.y = y * other.y;
			return vector;
		}

		auto operator*=(T value) -> Vector2<T>&
		{
			x *= value;
			y *= value;
			return *this;
		}

		auto operator*=(const Vector2<T>& other) -> Vector2<T>&
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		auto operator/(T value) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x / value;
			vector.y = y / value;
			return vector;
		}

		auto operator/(const Vector2<T>& other) const -> Vector2<T>
		{
			Vector2<T> vector;
			vector.x = x / other.x;
			vector.y = y / other.y;
			return vector;
		}

		auto operator/=(T value) -> Vector2<T>&
		{
			x /= value;
			y /= value;
			return *this;
		}

		auto operator/=(const Vector2<T>& other) -> Vector2<T>&
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}

	public:
		union
		{
			T data[2]{ 0 };

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

	using Vec2 = Vector2<float>;
	using Vec2f = Vector2<float>;
	using Vec2d = Vector2<double>;
	using Vec2i = Vector2<int>;
	using Vec2ui = Vector2<unsigned int>;
} // namespace HyperMath

HYPERENGINE_COMPILER_POP_WARNING
