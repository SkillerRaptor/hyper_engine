#pragma once

#include <iostream>
#include <cmath>

namespace Hyperion 
{
	class Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

	public:
		Vector4();
		Vector4(const Vector4& vector);
		Vector4(float x, float y, float z, float w);

		float magnitude() const;

		Vector4 operator-() const;

		Vector4 operator+(float value) const;
		Vector4 operator+(Vector4 vector) const;

		Vector4 operator-(float value) const;
		Vector4 operator-(Vector4 vector) const;

		Vector4 operator*(float value) const;
		Vector4 operator*(Vector4 vector) const;

		Vector4 operator/(float value) const;
		Vector4 operator/(Vector4 vector) const;

		Vector4& operator+=(float value);
		Vector4& operator+=(Vector4 vector);

		Vector4& operator-=(float value);
		Vector4& operator-=(Vector4 vector);

		Vector4& operator*=(float value);
		Vector4& operator*=(Vector4 vector);

		Vector4& operator/=(float value);
		Vector4& operator/=(Vector4 vector);

		friend std::ostream& operator<<(std::ostream& os, const Vector4& vector);
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector4& vector)
	{
		os << "X: " << vector.x << ", Y: " << vector.y << ", Z: " << vector.z << ", W: " << vector.w;
		return os;
	}
}