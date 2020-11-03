#include "Vector.hpp"

namespace Hyperion
{
	namespace Vector
	{
		Vec2 Normalize(Vec2& vector)
		{
			Vec2 vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			return vec;
		}

		Vec2 Normalize(const Vec2& vector)
		{
			Vec2 vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			return vec;
		}

		Vec3 Normalize(Vec3& vector)
		{
			Vec3 vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			return vec;
		}

		Vec3 Normalize(const Vec3& vector)
		{
			Vec3 vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			return vec;
		}

		Vec4 Normalize(Vec4& vector)
		{
			Vec4 vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			vec.w = vector.w / vector.Magnitude();
			return vec;
		}

		Vec4 Normalize(const Vec4& vector)
		{
			Vec4 vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			vec.w = vector.w / vector.Magnitude();
			return vec;
		}

		float Dot(Vec2& vectorOne, Vec2& vectorTwo)
		{
			Vec2 vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y;
		}

		float Dot(const Vec2& vectorOne, const Vec2& vectorTwo)
		{
			Vec2 vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y;
		}

		float Dot(Vec3& vectorOne, Vec3& vectorTwo)
		{
			Vec3 vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z;
		}

		float Dot(const Vec3& vectorOne, const Vec3& vectorTwo)
		{
			Vec3 vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z;
		}

		float Dot(Vec4& vectorOne, Vec4& vectorTwo)
		{
			Vec4 vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z + vec.w;
		}

		float Dot(const Vec4& vectorOne, const Vec4& vectorTwo)
		{
			Vec4 vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z + vec.w;
		}

		Vec3 Cross(Vec3& vectorOne, Vec3& vectorTwo)
		{
			Vec3 vec;
			vec.x = vectorOne.y * vectorTwo.z - vectorOne.z * vectorTwo.y;
			vec.y = vectorOne.z * vectorTwo.x - vectorOne.x * vectorTwo.z;
			vec.z = vectorOne.x * vectorTwo.y - vectorOne.y * vectorTwo.x;
			return vec;
		}

		Vec3 Cross(const Vec3& vectorOne, const Vec3& vectorTwo)
		{
			Vec3 vec;
			vec.x = vectorOne.y * vectorTwo.z - vectorOne.z * vectorTwo.y;
			vec.y = vectorOne.z * vectorTwo.x - vectorOne.x * vectorTwo.z;
			vec.z = vectorOne.x * vectorTwo.y - vectorOne.y * vectorTwo.x;
			return vec;
		}
		Vec2 Inverse(Vec2& vector)
		{
			return Vec2(-vector.x, -vector.y);
		}

		Vec2 Inverse(const Vec2& vector)
		{
			return Vec2(-vector.x, -vector.y);
		}

		Vec3 Inverse(const Vec3& vector)
		{
			return Vec3(-vector.x, -vector.y, -vector.z);
		}

		Vec3 Inverse(Vec3& vector)
		{
			return Vec3(-vector.x, -vector.y, -vector.z);
		}

		Vec4 Inverse(Vec4& vector)
		{
			return Vec4(-vector.x, -vector.y, -vector.z, -vector.w);
		}

		Vec4 Inverse(const Vec4& vector)
		{
			return Vec4(-vector.x, -vector.y, -vector.z, -vector.w);
		}
	}
}
