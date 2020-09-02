#pragma once

#include <iostream>
#include <cmath>

namespace Hyperion 
{
	class Vector2
	{
	public:
		float x;
		float y;

	public:
		Vector2();
		Vector2(const Vector2& vector);
		Vector2(float x, float y);

		float magnitude() const;

		Vector2 operator-() const;

		Vector2 operator+(float value) const;
		Vector2 operator+(const Vector2& vector) const;

		Vector2 operator-(float value) const;
		Vector2 operator-(const Vector2& vector) const;

		Vector2 operator*(float value) const;
		Vector2 operator*(const Vector2& vector) const;

		Vector2 operator/(float value) const;
		Vector2 operator/(const Vector2& vector) const;

		Vector2& operator+=(float value);
		Vector2& operator+=(const Vector2& vector);

		Vector2& operator-=(float value);
		Vector2& operator-=(const Vector2& vector);

		Vector2& operator*=(float value);
		Vector2& operator*=(const Vector2& vector);

		Vector2& operator/=(float value);
		Vector2& operator/=(const Vector2& vector);

		bool operator==(const Vector2& vector);
		bool operator!=(const Vector2& vector);

		friend std::ostream& operator<<(std::ostream& os, const Vector2& vector);
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector2& vector)
	{
		os << "X: " << vector.x << ", Y: " << vector.y;
		return os;
	}
}