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
		Vec2 Normalize(Vec2& vector);
		Vec2 Normalize(const Vec2& vector);
		Vec3 Normalize(Vec3& vector);
		Vec3 Normalize(const Vec3& vector);
		Vec4 Normalize(Vec4& vector);
		Vec4 Normalize(const Vec4& vector);

		float Dot(Vec2& vectorOne, Vec2& vectorTwo);
		float Dot(const Vec2& vectorOne, const Vec2& vectorTwo);
		float Dot(Vec3& vectorOne, Vec3& vectorTwo);
		float Dot(const Vec3& vectorOne, const Vec3& vectorTwo);
		float Dot(Vec4& vectorOne, Vec4& vectorTwo);
		float Dot(const Vec4& vectorOne, const Vec4& vectorTwo);

		Vec3 Cross(Vec3& vectorOne, Vec3& vectorTwo);
		Vec3 Cross(const Vec3& vectorOne, const Vec3& vectorTwo);
	};
}
