#pragma once

#include <iostream>
#include <cmath>

#include "Vector3.h"

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
		Matrix3 operator+(Matrix3 matrix3) const;
		Matrix3 operator+=(Matrix3 matrix3);
		Matrix3 operator+(Vector3& vector) const;
		Matrix3 operator+=(Vector3& vector);

		Matrix3 operator-(float value) const;
		Matrix3 operator-=(float value);
		Matrix3 operator-(Matrix3 matrix3) const;
		Matrix3 operator-=(Matrix3 matrix3);
		Matrix3 operator-(Vector3& vector) const;
		Matrix3 operator-=(Vector3& vector);

		Matrix3 operator*(float value) const;
		Matrix3 operator*=(float value);
		Matrix3 operator*(Matrix3 matrix3) const;
		Matrix3 operator*=(Matrix3 matrix3);
		Matrix3 operator*(Vector3& vector) const;
		Matrix3 operator*=(Vector3& vector);

		Matrix3 operator/(float value) const;
		Matrix3 operator/=(float value);
		Matrix3 operator/(Matrix3 matrix3) const;
		Matrix3 operator/=(Matrix3 matrix3);
		Matrix3 operator/(Vector3& vector) const;
		Matrix3 operator/=(Vector3& vector);

		float* operator[](size_t i) { return matrix[i]; }

		friend std::ostream& operator<<(std::ostream& os, const Matrix3& matrix);
	};

	inline std::ostream& operator<<(std::ostream& os, const Matrix3& matrix)
	{
		for (int i = 0; i < 3; i++)
		{
			os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << ", " << matrix.matrix[i][2] << ", " << matrix.matrix[i][3] << "]";
			if (i != 3)
				os << std::endl;
		}
		return os;
	}
}