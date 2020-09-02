#pragma once

#include <iostream>
#include <cmath>

#include "Vector2.hpp"

namespace Hyperion
{
	class Matrix2
	{
	public:
		float matrix[2][2];

	public:
		Matrix2();
		Matrix2(float value);
		Matrix2(const Matrix2& matrix2);

		Matrix2 operator+(float value) const;
		Matrix2 operator+=(float value);
		Matrix2 operator+(const Matrix2& matrix2) const;
		Matrix2 operator+=(const Matrix2& matrix2);
		Matrix2 operator+(const Vector2& vector) const;
		Matrix2 operator+=(const Vector2& vector);

		Matrix2 operator-(float value) const;
		Matrix2 operator-=(float value);
		Matrix2 operator-(const Matrix2& matrix2) const;
		Matrix2 operator-=(const Matrix2& matrix2);
		Matrix2 operator-(const Vector2& vector) const;
		Matrix2 operator-=(const Vector2& vector);

		Matrix2 operator*(float value) const;
		Matrix2 operator*=(float value);
		Matrix2 operator*(const Matrix2& matrix2) const;
		Matrix2 operator*=(const Matrix2& matrix2);
		Matrix2 operator*(const Vector2& vector) const;
		Matrix2 operator*=(const Vector2& vector);

		Matrix2 operator/(float value) const;
		Matrix2 operator/=(float value);
		Matrix2 operator/(const Matrix2& matrix2) const;
		Matrix2 operator/=(const Matrix2& matrix2);
		Matrix2 operator/(const Vector2& vector) const;
		Matrix2 operator/=(const Vector2& vector);

		bool operator==(const Matrix2& matrix2);
		bool operator!=(const Matrix2& matrix2);

		float* operator[](size_t i) { return matrix[i]; }

		friend std::ostream& operator<<(std::ostream& os, const Matrix2& matrix);
	};

	inline std::ostream& operator<<(std::ostream& os, const Matrix2& matrix)
	{
		for (int i = 0; i < 2; i++)
		{
			os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << "]";
			if (i != 3)
				os << std::endl;
		}
		return os;
	}
}