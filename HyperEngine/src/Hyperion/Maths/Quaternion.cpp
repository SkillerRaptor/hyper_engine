#include "Quaternion.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Hyperion
{
	Quaternion::Quaternion()
	{
	}

	Quaternion::Quaternion(float scalar, Vec3 axis)
		: m_Scalar(scalar), m_Axis(axis)
	{
	}

	Quaternion::~Quaternion()
	{
	}

	Quaternion::Quaternion(const Quaternion& other)
	{
		m_Scalar = other.m_Scalar;
		m_Axis = other.m_Axis;
	}

	Quaternion& Quaternion::operator=(const Quaternion& other)
	{
		m_Scalar = other.m_Scalar;
		m_Axis = other.m_Axis;

		return *this;
	}

	Quaternion Quaternion::operator+(const Quaternion& other) const
	{
		return Quaternion(m_Scalar + other.m_Scalar, m_Axis + other.m_Axis);
	}

	void Quaternion::operator+=(const Quaternion& other)
	{
		m_Scalar += other.m_Scalar;
		m_Axis += other.m_Axis;
	}

	Quaternion Quaternion::operator-(const Quaternion& other) const
	{
		return Quaternion(m_Scalar - other.m_Scalar, m_Axis - other.m_Axis);
	}

	void Quaternion::operator-=(const Quaternion& other)
	{
		m_Scalar -= other.m_Scalar;
		m_Axis -= other.m_Axis;
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		return Quaternion(m_Scalar * other.m_Scalar - Vector::Dot(m_Axis, other.m_Axis), other.m_Axis * m_Scalar + m_Axis * other.m_Scalar + Vector::Cross(m_Axis, other.m_Axis));
	}

	void Quaternion::operator*=(const Quaternion& other)
	{
		m_Scalar = m_Scalar * other.m_Scalar - Vector::Dot(m_Axis, other.m_Axis);
		m_Axis = other.m_Axis * m_Scalar + m_Axis * other.m_Scalar + Vector::Cross(m_Axis, other.m_Axis);
	}

	Quaternion Quaternion::operator*(const float value) const
	{
		return Quaternion(m_Scalar * value, m_Axis * value);
	}

	void Quaternion::operator*=(const float value)
	{
		m_Scalar *= value;
		m_Axis *= value;
	}

	float Quaternion::Norm(Quaternion quaternion)
	{
		Vec3 axis = quaternion.m_Axis * quaternion.m_Axis;
		return sqrt(quaternion.m_Scalar * quaternion.m_Scalar + axis.x + axis.y + axis.z);
	}

	Quaternion Quaternion::Normalize(Quaternion quaternion)
	{
		if (Norm(quaternion) == 0)
			return quaternion;
		float normValue = 1 / Norm(quaternion);
		quaternion.m_Scalar *= normValue;
		quaternion.m_Axis *= normValue;
		return quaternion;
	}

	Quaternion Quaternion::ConvertToUnitNorm(Quaternion quaternion)
	{
		float angle = (float) quaternion.m_Scalar * ((float) M_PI) / 180;
		quaternion.m_Axis = Vector::Normalize(quaternion.m_Axis);
		return Quaternion(cosf(angle * 0.5f), quaternion.m_Axis * sinf(angle * 0.5f));
	}

	Quaternion Quaternion::Conjugate(Quaternion quaternion)
	{
		return Quaternion(quaternion.m_Scalar, quaternion.m_Axis * -1);
	}

	Quaternion Quaternion::Inverse(Quaternion quaternion)
	{
		float absoluteValue = Norm(quaternion);
		absoluteValue *= absoluteValue;
		absoluteValue = 1 / absoluteValue;

		return Quaternion(Conjugate(quaternion).m_Scalar * absoluteValue, Conjugate(quaternion).m_Axis * absoluteValue);
	}

	Vec3 Quaternion::RotateVector(Vec3 vector, float angle, Vec3 axis)
	{
		Quaternion pure = Quaternion(0, vector);
		axis = Vector::Normalize(axis);
		Quaternion real = Quaternion(angle, axis);
		real = ConvertToUnitNorm(real);
		Quaternion inverse = Quaternion::Inverse(real);
		Quaternion rotated = real * pure * inverse;
		return rotated.m_Axis;
	}
}
