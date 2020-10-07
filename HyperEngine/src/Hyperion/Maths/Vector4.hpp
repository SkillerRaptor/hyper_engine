#pragma once

#include <cmath>
#include <iostream>
#include <type_traits>

#include "Vector2.hpp"
#include "Vector3.hpp"

namespace Hyperion 
{
	template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	class Vector4
	{
	public:
		T x, y, z, w;

	public:
		Vector4()
			: x(0), y(0), z(0), w(0) {}
		Vector4(const Vector4<T>& xyzw)
			: x(xyzw.x), y(xyzw.y), z(xyzw.z), w(xyzw.w) {}
		Vector4(T xyzw)
			: x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
		Vector4(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w) {}
		Vector4(const Vector2<T>& xy, T z, T w)
			: x(xy.x), y(xy.y), z(z), w(w) {}
		Vector4(T x, const Vector2<T>& yz, T w)
			: x(x), y(yz.y), z(yz.z), w(w) {}
		Vector4(T x, T y, const Vector2<T>& zw)
			: x(x), y(y), z(zw.z), w(zw.w) {}
		Vector4(const Vector2<T>& xy, const Vector2<T>& zw)
			: x(xy.x), y(xy.y), z(zw.z), w(zw.w) {}
		Vector4(const Vector3<T>& xyz, T w)
			: x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
		Vector4(T x, const Vector3<T>& yzw)
			: x(x), y(yzw.y), z(yzw.z), w(yzw.w) {}

		float Magnitude() const
		{
			return sqrt(x * x + y * y + z * z + w * w);
		}

		/* Negate */
		Vector4<T> operator-() const
		{
			Vector4<T> vec;
			vec.x = -x;
			vec.y = -y;
			vec.z = -z;
			vec.w = -w;
			return vec;
		}

		/* Adding */
		Vector4<T> operator+(T value) const
		{
			Vector4<T> vec;
			vec.x = x + value;
			vec.y = y + value;
			vec.z = z + value;
			vec.w = w + value;
			return vec;
		}

		Vector4<T> operator+(const Vector4<T>& vector) const
		{
			Vector4<T> vec;
			vec.x = x + vector.x;
			vec.y = y + vector.y;
			vec.z = z + vector.z;
			vec.w = w + vector.w;
			return vec;
		}

		Vector4<T>& operator+=(T value)
		{
			x += value;
			y += value;
			z += value;
			w += value;
			return *this;
		}

		Vector4<T>& operator+=(const Vector4<T>& vector)
		{
			x += vector.x;
			y += vector.y;
			z += vector.z;
			w += vector.w;
			return *this;
		}

		/* Subtracting */
		Vector4<T> operator-(T value) const
		{
			Vector4<T> vec;
			vec.x = x - value;
			vec.y = y - value;
			vec.z = z - value;
			vec.w = w - value;
			return vec;
		}

		Vector4<T> operator-(const Vector4<T>& vector) const
		{
			Vector4<T> vec;
			vec.x = x - vector.x;
			vec.y = y - vector.y;
			vec.z = z - vector.z;
			vec.w = w - vector.w;
			return vec;
		}

		Vector4<T>& operator-=(T value)
		{
			x -= value;
			y -= value;
			z -= value;
			w -= value;
			return *this;
		}

		Vector4<T>& operator-=(const Vector4<T>& vector)
		{
			x -= vector.x;
			y -= vector.y;
			z -= vector.z;
			w -= vector.w;
			return *this;
		}

		/* Multiplying */
		Vector4<T> operator*(T value) const
		{
			Vector4<T> vec;
			vec.x = x * value;
			vec.y = y * value;
			vec.z = z * value;
			vec.w = w * value;
			return vec;
		}

		Vector4<T> operator*(const Vector4<T>& vector) const
		{
			Vector4<T> vec;
			vec.x = x * vector.x;
			vec.y = y * vector.y;
			vec.z = z * vector.z;
			vec.w = w * vector.w;
			return vec;
		}

		Vector4<T>& operator*=(T value)
		{
			x *= value;
			y *= value;
			z *= value;
			w *= value;
			return *this;
		}

		Vector4<T>& operator*=(const Vector4<T>& vector)
		{
			x *= vector.x;
			y *= vector.y;
			z *= vector.z;
			w *= vector.w;
			return *this;
		}

		/* Dividing */
		Vector4<T> operator/(T value) const
		{
			Vector4<T> vec;
			vec.x = x / value;
			vec.y = y / value;
			vec.z = z / value;
			vec.w = w / value;
			return vec;
		}

		Vector4<T> operator/(const Vector4<T>& vector) const
		{
			Vector4<T> vec;
			vec.x = x / vector.x;
			vec.y = y / vector.y;
			vec.z = z / vector.z;
			vec.w = w / vector.w;
			return vec;
		}

		Vector4<T>& operator/=(T value)
		{
			x /= value;
			y /= value;
			z /= value;
			w /= value;
			return *this;
		}

		Vector4<T>& operator/=(const Vector4<T>& vector)
		{
			x /= vector.x;
			y /= vector.y;
			z /= vector.z;
			w /= vector.w;
			return *this;
		}

		/* Comparsion */
		bool operator==(const Vector4<T>& vector)
		{
			return (x == vector.x && y == vector.y && z == vector.z && w == vector.w);
		}

		bool operator!=(const Vector4<T>& vector)
		{
			return (x != vector.x || y != vector.y || z != vector.z || w != vector.w);
		}

		/* Conversion */
		operator float* ()
		{
			return reinterpret_cast<float*>(this);
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector4<T>& vector);
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Vector4<T>& vector)
	{
		os << "X: " << vector.x << ", Y: " << vector.y << ", Z: " << vector.z << ", W: " << vector.w;
		return os;
	}
}
