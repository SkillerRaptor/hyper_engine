#include "Vector3.h"

namespace Hyperion 
{
	Vector3::Vector3()
		: x(0), y(0), z(0)
	{
	}

	Vector3::Vector3(const Vector3& vector)
		: x(vector.x), y(vector.y), z(vector.z)
	{
	}
	
	Vector3::Vector3(float x, float y, float z)
		: x(x), y(y), z(z)
	{
	}

	float Vector3::magnitude() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 Vector3::operator-() const
	{
		Vector3 vec;
		vec.x = -x;
		vec.y = -y;
		vec.z = -z;
		return vec;
	}

	Vector3 Vector3::operator+(float value) const
	{
		Vector3 vec;
		vec.x = x + value;
		vec.y = y + value;
		vec.z = z + value;
		return vec;
	}

	Vector3 Vector3::operator+(const Vector3& vector) const
	{
		Vector3 vec;
		vec.x = x + vector.x;
		vec.y = y + vector.y;
		vec.z = z + vector.z;
		return vec;
	}

	Vector3 Vector3::operator-(float value) const
	{
		Vector3 vec;
		vec.x = x - value;
		vec.y = y - value;
		vec.z = z - value;
		return vec;
	}

	Vector3 Vector3::operator-(const Vector3& vector) const
	{
		Vector3 vec;
		vec.x = x - vector.x;
		vec.y = y - vector.y;
		vec.z = z - vector.z;
		return vec;
	}

	Vector3 Vector3::operator*(float value) const
	{
		Vector3 vec;
		vec.x = x * value;
		vec.y = y * value;
		vec.z = z * value;
		return vec;
	}

	Vector3 Vector3::operator*(const Vector3& vector) const
	{
		Vector3 vec;
		vec.x = x * vector.x;
		vec.y = y * vector.y;
		vec.z = z * vector.z;
		return vec;
	}

	Vector3 Vector3::operator/(float value) const
	{
		Vector3 vec;
		vec.x = x / value;
		vec.y = y / value;
		vec.z = z / value;
		return vec;
	}

	Vector3 Vector3::operator/(const Vector3& vector) const
	{
		Vector3 vec;
		vec.x = x / vector.x;
		vec.y = y / vector.y;
		vec.z = z / vector.z;
		return vec;
	}

	Vector3& Vector3::operator+=(float value)
	{
		x += value;
		y += value;
		z += value;
		return *this;
	}

	Vector3& Vector3::operator+=(const Vector3& vector)
	{
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}

	Vector3& Vector3::operator-=(float value)
	{
		x -= value;
		y -= value;
		z -= value;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& vector)
	{
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}

	Vector3& Vector3::operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	Vector3& Vector3::operator*=(const Vector3& vector)
	{
		x *= vector.x;
		y *= vector.y;
		z *= vector.z;
		return *this;
	}

	Vector3& Vector3::operator/=(float value)
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	Vector3& Vector3::operator/=(const Vector3& vector)
	{
		x /= vector.x;
		y /= vector.y;
		z /= vector.z;
		return *this;
	}

	bool Vector3::operator==(const Vector3& vector)
	{
		return (x == vector.x && y == vector.y && z == vector.z);
	}

	bool Vector3::operator!=(const Vector3& vector)
	{
		return (x != vector.x && y != vector.y && z != vector.z);
	}
}