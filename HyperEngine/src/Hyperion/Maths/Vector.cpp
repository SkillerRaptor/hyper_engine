#include "Vector.hpp"

namespace Hyperion 
{
	Vector2 Vector::normalize(Vector2& vector)
	{
		Vector2 vec;
		vec.x = vector.x / vector.magnitude();
		vec.y = vector.y / vector.magnitude();
		return vec;
	}

	Vector3 Vector::normalize(Vector3& vector)
	{
		Vector3 vec;
		vec.x = vector.x / vector.magnitude();
		vec.y = vector.y / vector.magnitude();
		vec.z = vector.z / vector.magnitude();
		return vec;
	}

	Vector4 Vector::normalize(Vector4& vector)
	{
		Vector4 vec;
		vec.x = vector.x / vector.magnitude();
		vec.y = vector.y / vector.magnitude();
		vec.z = vector.z / vector.magnitude();
		vec.w = vector.w / vector.magnitude();
		return vec;
	}

	float Vector::dot(Vector2& vectorOne, Vector2& vectorTwo)
	{
		Vector2 vec = normalize(vectorOne) * normalize(vectorTwo);
		return vec.x + vec.y;
	}

	float Vector::dot(Vector3& vectorOne, Vector3& vectorTwo)
	{
		Vector3 vec = normalize(vectorOne) * normalize(vectorTwo);
		return vec.x + vec.y + vec.z;
	}

	float Vector::dot(Vector4& vectorOne, Vector4& vectorTwo)
	{
		Vector4 vec = normalize(vectorOne) * normalize(vectorTwo);
		return vec.x + vec.y + vec.z + vec.w;
	}

	Vector3 Vector::cross(Vector3& vectorOne, Vector3& vectorTwo)
	{
		Vector3 vec;
		vec.x = vectorOne.y * vectorTwo.z - vectorOne.z * vectorTwo.y;
		vec.y = vectorOne.z * vectorTwo.x - vectorOne.x * vectorTwo.z;
		vec.z = vectorOne.x * vectorTwo.y - vectorOne.y * vectorTwo.x;
		return vec;
	}
}