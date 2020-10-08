#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Hyperion 
{
	typedef Vector2<signed char> ByteVec2;
	typedef Vector3<signed char> ByteVec3;
	typedef Vector4<signed char> ByteVec4;
	typedef Vector2<unsigned char> UByteVec2;
	typedef Vector3<unsigned char> UByteVec3;
	typedef Vector4<unsigned char> UByteVec4;
	typedef Vector2<short> ShortVec2;
	typedef Vector3<short> ShortVec3;
	typedef Vector4<short> ShortVec4;
	typedef Vector2<unsigned short> UShortVec2;
	typedef Vector3<unsigned short> UShortVec3;
	typedef Vector4<unsigned short> UShortVec4;
	typedef Vector2<int> IntVec2;
	typedef Vector3<int> IntVec3;
	typedef Vector4<int> IntVec4;
	typedef Vector2<unsigned int> UIntVec2;
	typedef Vector3<unsigned int> UIntVec3;
	typedef Vector4<unsigned int> UIntVec4;
	typedef Vector2<long> LongVec2;
	typedef Vector3<long> LongVec3;
	typedef Vector4<long> LongVec4;
	typedef Vector2<unsigned long> ULongVec2;
	typedef Vector3<unsigned long> ULongVec3;
	typedef Vector4<unsigned long> ULongVec4;
	typedef Vector2<float> Vec2;
	typedef Vector3<float> Vec3;
	typedef Vector4<float> Vec4;
	typedef Vector2<double> DoubleVec2;
	typedef Vector3<double> DoubleVec3;
	typedef Vector4<double> DoubleVec4;
	typedef Vector2<long double> LDoubleVec2;
	typedef Vector3<long double> LDoubleVec3;
	typedef Vector4<long double> LDoubleVec4;

	namespace Vector
	{
		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector2<T> Normalize(Vector2<T>& vector)
		{
			Vector2<T> vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			return vec;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector3<T> Normalize(Vector3<T>& vector)
		{
			Vector3<T> vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			return vec;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector4<T> Normalize(Vector4<T>& vector)
		{
			Vector4<T> vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			vec.w = vector.w / vector.Magnitude();
			return vec;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		float Dot(Vector2<T>& vectorOne, Vector2<T>& vectorTwo)
		{
			Vector2<T> vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		float Dot(Vector3<T>& vectorOne, Vector3<T>& vectorTwo)
		{
			Vector3<T> vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		float Dot(Vector4<T>& vectorOne, Vector4<T>& vectorTwo)
		{
			Vector4<T> vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z + vec.w;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector3<T> Cross(Vector3<T>& vectorOne, Vector3<T>& vectorTwo)
		{
			Vector3<T> vec;
			vec.x = vectorOne.y * vectorTwo.z - vectorOne.z * vectorTwo.y;
			vec.y = vectorOne.z * vectorTwo.x - vectorOne.x * vectorTwo.z;
			vec.z = vectorOne.x * vectorTwo.y - vectorOne.y * vectorTwo.x;
			return vec;
		}
	};
}
