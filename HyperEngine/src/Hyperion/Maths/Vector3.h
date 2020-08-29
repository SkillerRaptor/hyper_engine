#pragma once

#include <iostream>
#include <cmath>

namespace Hyperion 
{
	class Vector3
	{
	public:
		float x;
		float y;
		float z;

	public:
		Vector3();
		Vector3(const Vector3& vector);
		Vector3(float x, float y, float z);

		float magnitude() const;

		Vector3 operator-() const;

		Vector3 operator+(float value) const;
		Vector3 operator+(Vector3 vector) const;

		Vector3 operator-(float value) const;
		Vector3 operator-(Vector3 vector) const;

		Vector3 operator*(float value) const;
		Vector3 operator*(Vector3 vector) const;

		Vector3 operator/(float value) const;
		Vector3 operator/(Vector3 vector) const;

		Vector3& operator+=(float value);
		Vector3& operator+=(Vector3 vector);

		Vector3& operator-=(float value);
		Vector3& operator-=(Vector3 vector);

		Vector3& operator*=(float value);
		Vector3& operator*=(Vector3 vector);

		Vector3& operator/=(float value);
		Vector3& operator/=(Vector3 vector);

		friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector3& vector)
	{
		os << "X: " << vector.x << ", Y: " << vector.y << ", Z: " << vector.z;
		return os;
	}
}