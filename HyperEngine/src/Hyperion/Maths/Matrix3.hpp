#pragma once

#include <iostream>
#include <cmath>

#include "Vector3.hpp"

namespace Hyperion 
{
	class Matrix3
	{
	public:
		float matrix[3][3];

	public:
		Matrix3();
		Matrix3(float value);
		Matrix3(const Matrix3& matrix3);

		Matrix3 operator+(float value) const;
		Matrix3 operator+=(float value);
		Matrix3 operator+(const Matrix3& matrix3) const;
		Matrix3 operator+=(const Matrix3& matrix3);
		Matrix3 operator+(const Vector3<float>& vector) const;
		Matrix3 operator+=(const Vector3<float>& vector);

		Matrix3 operator-(float value) const;
		Matrix3 operator-=(float value);
		Matrix3 operator-(const Matrix3& matrix3) const;
		Matrix3 operator-=(const Matrix3& matrix3);
		Matrix3 operator-(const Vector3<float>& vector) const;
		Matrix3 operator-=(const Vector3<float>& vector);

		Matrix3 operator*(float value) const;
		Matrix3 operator*=(float value);
		Matrix3 operator*(const Matrix3& matrix3) const;
		Matrix3 operator*=(const Matrix3& matrix3);
		Matrix3 operator*(const Vector3<float>& vector) const;
		Matrix3 operator*=(const Vector3<float>& vector);

		Matrix3 operator/(float value) const;
		Matrix3 operator/=(float value);
		Matrix3 operator/(const Matrix3& matrix3) const;
		Matrix3 operator/=(const Matrix3& matrix3);
		Matrix3 operator/(const Vector3<float>& vector) const;
		Matrix3 operator/=(const Vector3<float>& vector);

		bool operator==(const Matrix3& matrix4);
		bool operator!=(const Matrix3& matrix4);

		float* operator[](size_t i) { return matrix[i]; }

		friend std::ostream& operator<<(std::ostream& os, const Matrix3& matrix);
	};

	inline std::ostream& operator<<(std::ostream& os, const Matrix3& matrix)
	{
		for (int i = 0; i < 3; i++)
		{
			os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << ", " << matrix.matrix[i][2] << "]";
			if (i != 3)
				os << std::endl;
		}
		return os;
	}
}