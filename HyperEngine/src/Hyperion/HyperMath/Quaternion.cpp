#include "Quaternion.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Hyperion
{
	Quaternion::Quaternion()
	{
	}

	Quaternion::Quaternion(Vec3 vector)
	{
		float Roll = (float)vector.x * ((float)M_PI) / 180;
		float Yaw = (float) vector.y * ((float)M_PI) / 180;
		float Pitch = (float)vector.z * ((float)M_PI) / 180;

		float C1 = cos(Yaw / 2);
		float C2 = cos(Pitch / 2);
		float C3 = cos(Roll / 2);
		float S1 = sin(Yaw / 2);
		float S2 = sin(Pitch / 2);
		float S3 = sin(Roll / 2);

		m_Scalar = (C1 * C2 * C3) - (S1 * S2 * S3);
		m_Axis = Vec3(
			(S1 * S2 * C3) + (C1 * C2 * S3), 
			(S1 * C2 * C3) + (C1 * S2 * S3), 
			(C1 * S2 * C3) - (S1 * C2 * S3));
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

	Quaternion Quaternion::RotateQuaternion(float angle, Vec3 axis)
	{
		float radians = (float) angle * ((float)M_PI) / 180;
		float factor = sin(radians / 2.0f);
		Vec3 rotationAxis;
		rotationAxis.x = axis.x * factor;
		rotationAxis.y = axis.y * factor;
		rotationAxis.z = axis.z * factor;
		float w = cos(radians / 2.0f);
		return Normalize(Quaternion(w, rotationAxis));
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

	Mat4 Quaternion::ConvertToMatrix(Vec4 vector)
	{
		Mat4 matrix(1.0f);
		matrix[0][0] = 1.0f - 2.0f * vector.y * vector.y - 2.0f * vector.z * vector.z;
		matrix[0][1] = 2.0f * vector.x * vector.y - 2.0f * vector.z * vector.w;
		matrix[0][2] = 2.0f * vector.x * vector.z + 2.0f * vector.y * vector.w;
		
		matrix[1][0] = 2.0f * vector.x * vector.y + 2.0f * vector.z * vector.w;
		matrix[1][1] = 1.0f - 2.0f * vector.x * vector.x - 2.0f * vector.z * vector.z;
		matrix[1][2] = 2.0f * vector.y * vector.z - 2.0f * vector.x * vector.w;
		
		matrix[2][0] = 2.0f * vector.x * vector.z - 2.0f * vector.y * vector.w;
		matrix[2][1] = 2.0f * vector.y * vector.z + 2.0f * vector.x * vector.w;
		matrix[2][2] = 1.0f - 2.0f * vector.x * vector.x - 2.0f * vector.y * vector.y;
		return matrix;
	}

	Mat4 Quaternion::ConvertToMatrix(Quaternion quaternion)
	{
		Mat4 matrix(1.0f);
		matrix[0][0] = (quaternion.m_Scalar * quaternion.m_Scalar) + (quaternion.m_Axis.x * quaternion.m_Axis.x) - (quaternion.m_Axis.y * quaternion.m_Axis.y) - (quaternion.m_Axis.z * quaternion.m_Axis.z);
		matrix[0][1] = (2.0f * quaternion.m_Axis.x * quaternion.m_Axis.y) + (2.0f * quaternion.m_Scalar * quaternion.m_Axis.z);
		matrix[0][2] = (2.0f * quaternion.m_Axis.x * quaternion.m_Axis.z) - (2.0f * quaternion.m_Scalar * quaternion.m_Axis.y);

		matrix[1][0] = (2.0f * quaternion.m_Axis.x * quaternion.m_Axis.y) - (2.0f * quaternion.m_Scalar * quaternion.m_Axis.z);
		matrix[1][1] = (quaternion.m_Scalar * quaternion.m_Scalar) - (quaternion.m_Axis.x * quaternion.m_Axis.x) + (quaternion.m_Axis.y * quaternion.m_Axis.y) - (quaternion.m_Axis.z * quaternion.m_Axis.z);
		matrix[1][2] = (2.0f * quaternion.m_Axis.y * quaternion.m_Axis.z) + (2.0f * quaternion.m_Scalar * quaternion.m_Axis.x);

		matrix[2][0] = (2.0f * quaternion.m_Axis.x * quaternion.m_Axis.z) + (2.0f * quaternion.m_Scalar * quaternion.m_Axis.y);
		matrix[2][1] = (2.0f * quaternion.m_Axis.y * quaternion.m_Axis.z) - (2.0f * quaternion.m_Scalar * quaternion.m_Axis.x);
		matrix[2][2] = (quaternion.m_Scalar * quaternion.m_Scalar) - (quaternion.m_Axis.x * quaternion.m_Axis.x) - (quaternion.m_Axis.y * quaternion.m_Axis.y) + (quaternion.m_Axis.z * quaternion.m_Axis.z);
		return matrix;
	}
}
