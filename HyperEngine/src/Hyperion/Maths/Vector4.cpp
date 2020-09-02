#include "Vector4.h"

namespace Hyperion 
{
	Vector4::Vector4()
		: x(0), y(0), z(0), w(0)
	{
	}

	Vector4::Vector4(const Vector4& vector)
		: x(vector.x), y(vector.y), z(vector.z), w(vector.w)
	{
	}

	Vector4::Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{
	}

	float Vector4::magnitude() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	Vector4 Vector4::operator-() const
	{
		Vector4 vec;
		vec.x = -x;
		vec.y = -y;
		vec.z = -z;
		vec.w = -w;
		return vec;
	}

	Vector4 Vector4::operator+(float value) const
	{
		Vector4 vec;
		vec.x = x + value;
		vec.y = y + value;
		vec.z = z + value;
		vec.w = w + value;
		return vec;
	}

	Vector4 Vector4::operator+(const Vector4& vector) const
	{
		Vector4 vec;
		vec.x = x + vector.x;
		vec.y = y + vector.y;
		vec.z = z + vector.z;
		vec.w = w + vector.w;
		return vec;
	}

	Vector4 Vector4::operator-(float value) const
	{
		Vector4 vec;
		vec.x = x - value;
		vec.y = y - value;
		vec.z = z - value;
		vec.w = w - value;
		return vec;
	}

	Vector4 Vector4::operator-(const Vector4& vector) const
	{
		Vector4 vec;
		vec.x = x - vector.x;
		vec.y = y - vector.y;
		vec.z = z - vector.z;
		vec.w = w - vector.w;
		return vec;
	}

	Vector4 Vector4::operator*(float value) const
	{
		Vector4 vec;
		vec.x = x * value;
		vec.y = y * value;
		vec.z = z * value;
		vec.w = w * value;
		return vec;
	}

	Vector4 Vector4::operator*(const Vector4& vector) const
	{
		Vector4 vec;
		vec.x = x * vector.x;
		vec.y = y * vector.y;
		vec.z = z * vector.z;
		vec.w = w * vector.w;
		return vec;
	}

	Vector4 Vector4::operator/(float value) const
	{
		Vector4 vec;
		vec.x = x / value;
		vec.y = y / value; 
		vec.z = z / value;
		vec.w = w / value;
		return vec;
	}

	Vector4 Vector4::operator/(const Vector4& vector) const
	{
		Vector4 vec;
		vec.x = x / vector.x;
		vec.y = y / vector.y;
		vec.z = z / vector.z;
		vec.w = w / vector.w;
		return vec;
	}

	Vector4& Vector4::operator+=(float value)
	{
		x += value;
		y += value;
		z += value;
		w += value;
		return *this;
	}

	Vector4& Vector4::operator+=(const Vector4& vector)
	{
		x += vector.x;
		y += vector.y;
		z += vector.z;
		w += vector.w;
		return *this;
	}

	Vector4& Vector4::operator-=(float value)
	{
		x -= value;
		y -= value;
		z -= value;
		w -= value;
		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& vector)
	{
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		w -= vector.w;
		return *this;
	}

	Vector4& Vector4::operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
		return *this;
	}

	Vector4& Vector4::operator*=(const Vector4& vector)
	{
		x *= vector.x;
		y *= vector.y;
		z *= vector.z;
		w *= vector.w;
		return *this;
	}

	Vector4& Vector4::operator/=(float value)
	{
		x /= value;
		y /= value;
		z /= value;
		w /= value;
		return *this;
	}

	Vector4& Vector4::operator/=(const Vector4& vector)
	{
		x /= vector.x;
		y /= vector.y;
		z /= vector.z;
		w /= vector.w;
		return *this;
	}

	bool Vector4::operator==(const Vector4& vector)
	{
		return (x == vector.x && y == vector.y && z == vector.z && w == vector.w);
	}

	bool Vector4::operator!=(const Vector4& vector)
	{
		return (x != vector.x && y != vector.y && z != vector.z && w != vector.w);
	}
}