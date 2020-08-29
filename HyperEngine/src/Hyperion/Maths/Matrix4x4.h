#pragma once

#include <iostream>
#include <cmath>

#include "Vector3.h"
#include "Vector4.h"

namespace Hyperion {

    class Matrix4x4
    {
    public:
        float matrix[4][4];

    public:
        Matrix4x4();
        Matrix4x4(float value);
        Matrix4x4(const Matrix4x4& matrix4x4);

		Matrix4x4 operator+(float value) const;
		Matrix4x4 operator+(Matrix4x4 matrix4x4) const;

		Matrix4x4 operator-(float value) const;
		Matrix4x4 operator-(Matrix4x4 matrix4x4) const;

        Matrix4x4 operator*(float value) const;
		Matrix4x4 operator*(Matrix4x4 matrix4x4) const;

		Matrix4x4 operator/(float value) const;
		Matrix4x4 operator/(Matrix4x4 matrix4x4) const;

		Matrix4x4 operator+=(float value);
		Matrix4x4 operator+=(Matrix4x4 matrix4x4);
		Matrix4x4 operator-=(float value);
		Matrix4x4 operator-=(Matrix4x4 matrix4x4);
		Matrix4x4 operator*=(float value);
		Matrix4x4 operator*=(Matrix4x4 matrix4x4);
		Matrix4x4 operator/=(float value);
		Matrix4x4 operator/=(Matrix4x4 matrix4x4);

		Matrix4x4 operator+(Vector4& vector) const;
		Matrix4x4 operator+=(Vector4& vector);

		Matrix4x4 operator-(Vector4& vector) const;
		Matrix4x4 operator-=(Vector4& vector);

		Matrix4x4 operator*(Vector4& vector) const;
		Matrix4x4 operator*=(Vector4& vector);

		Matrix4x4 operator/(Vector4& vector) const;
		Matrix4x4 operator/=(Vector4& vector);

        friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& matrix);
	};

	std::ostream& operator<<(std::ostream& os, const Matrix4x4& matrix)
	{
		for (int i = 0; i < 4; i++)
		{
			os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << ", " << matrix.matrix[i][2] << ", " << matrix.matrix[i][3] << "]";
			if (i != 3)
				os << std::endl;
		}
		return os;
	}
}