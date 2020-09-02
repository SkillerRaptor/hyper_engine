#include "Vector2.h"

namespace Hyperion 
{
	Vector2::Vector2()
		: x(0), y(0)
	{
	}

	Vector2::Vector2(const Vector2& vector)
		: x(vector.x), y(vector.y)
	{
	}

	Vector2::Vector2(float x, float y)
		: x(x), y(y)
	{
	}

	float Vector2::magnitude() const
	{
		return sqrt(x * x + y * y);
	}

	Vector2 Vector2::operator-() const
	{
		Vector2 vec;
		vec.x = -x;
		vec.y = -y;
		return vec;
	}

	Vector2 Vector2::operator+(float value) const
	{
		Vector2 vec;
		vec.x = x + value;
		vec.y = y + value;
		return vec;
	}

	Vector2 Vector2::operator+(const Vector2& vector) const
	{
		Vector2 vec;
		vec.x = x + vector.x;
		vec.y = y + vector.y;
		return vec;
	}

	Vector2 Vector2::operator-(float value) const
	{
		Vector2 vec;
		vec.x = x - value;
		vec.y = y - value;
		return vec;
	}

	Vector2 Vector2::operator-(const Vector2& vector) const
	{
		Vector2 vec;
		vec.x = x - vector.x;
		vec.y = y - vector.y;
		return vec;
	}

	Vector2 Vector2::operator*(float value) const
	{
		Vector2 vec;
		vec.x = x * value;
		vec.y = y * value;
		return vec;
	}

	Vector2 Vector2::operator*(const Vector2& vector) const
	{
		Vector2 vec;
		vec.x = x * vector.x;
		vec.y = y * vector.y;
		return vec;
	}

	Vector2 Vector2::operator/(float value) const
	{
		Vector2 vec;
		vec.x = x / value;
		vec.y = y / value;
		return vec;
	}

	Vector2 Vector2::operator/(const Vector2& vector) const
	{
		Vector2 vec;
		vec.x = x / vector.x;
		vec.y = y / vector.y;
		return vec;
	}

	Vector2& Vector2::operator+=(float value)
	{
		x += value;
		y += value;
		return *this;
	}

	Vector2& Vector2::operator+=(const Vector2& vector)
	{
		x += vector.x;
		y += vector.y;
		return *this;
	}

	Vector2& Vector2::operator-=(float value)
	{
		x -= value;
		y -= value;
		return *this;
	}

	Vector2& Vector2::operator-=(const Vector2& vector)
	{
		x -= vector.x;
		y -= vector.y;
		return *this;
	}

	Vector2& Vector2::operator*=(float value)
	{
		x *= value;
		y *= value;
		return *this;
	}

	Vector2& Vector2::operator*=(const Vector2& vector)
	{
		x *= vector.x;
		y *= vector.y;
		return *this;
	}

	Vector2& Vector2::operator/=(float value)
	{
		x /= value;
		y /= value;
		return *this;
	}

	Vector2& Vector2::operator/=(const Vector2& vector)
	{
		x /= vector.x;
		y /= vector.y;
		return *this;
	}

	bool Vector2::operator==(const Vector2& vector)
	{
		return (x == vector.x && y == vector.y);
	}

	bool Vector2::operator!=(const Vector2& vector)
	{
		return (x != vector.x && y != vector.y);
	}
}