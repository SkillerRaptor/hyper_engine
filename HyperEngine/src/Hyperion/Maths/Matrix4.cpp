#include "Matrix4.h"

namespace Hyperion 
{
	Matrix4::Matrix4()
		: Matrix4(0.0f)
	{
	}

	Matrix4::Matrix4(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				matrix[i][j] = 0.0f;
				if (i == j)
					matrix[i][j] = value;
			}
	}

	Matrix4::Matrix4(const Matrix4& matrix4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] = matrix4.matrix[i][j];
	}

	/* Matrix Addition Operators */

	Matrix4 Matrix4::operator+(float value) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] + value;
		return mat;
	}

	Matrix4 Matrix4::operator+=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] += value;
		return *this;
	}

	Matrix4 Matrix4::operator+(Matrix4 matrix4) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] + matrix4.matrix[i][j];
		return mat;
	}

	Matrix4 Matrix4::operator+=(Matrix4 matrix4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] += matrix4.matrix[i][j];
		return *this;
	}

	Matrix4 Matrix4::operator+(Vector4& vector) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] + vector.x;
			mat.matrix[i][1] = matrix[i][1] + vector.y;
			mat.matrix[i][2] = matrix[i][2] + vector.z;
			mat.matrix[i][3] = matrix[i][3] + vector.w;
		}
		return mat;
	}

	Matrix4 Matrix4::operator+=(Vector4& vector)
	{
		for (int i = 0; i < 4; i++)
		{
			matrix[i][0] += vector.x;
			matrix[i][1] += vector.y;
			matrix[i][2] += vector.z;
			matrix[i][3] += vector.w;
		}
		return *this;
	}

	/* Matrix Subtraction Operators */

	Matrix4 Matrix4::operator-(float value) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] - value;
		return mat;
	}

	Matrix4 Matrix4::operator-=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] -= value;
		return *this;
	}

	Matrix4 Matrix4::operator-(Matrix4 matrix4) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] - matrix4.matrix[i][j];
		return mat;
	}

	Matrix4 Matrix4::operator-=(Matrix4 matrix4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] -= matrix4.matrix[i][j];
		return *this;
	}

	Matrix4 Matrix4::operator-(Vector4& vector) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] - vector.x;
			mat.matrix[i][1] = matrix[i][1] - vector.y;
			mat.matrix[i][2] = matrix[i][2] - vector.z;
			mat.matrix[i][3] = matrix[i][3] - vector.w;
		}
		return mat;
	}

	Matrix4 Matrix4::operator-=(Vector4& vector)
	{
		for (int i = 0; i < 4; i++)
		{
			matrix[i][0] -= vector.x;
			matrix[i][1] -= vector.y;
			matrix[i][2] -= vector.z;
			matrix[i][3] -= vector.w;
		}
		return *this;
	}

	/* Matrix Multiplication Operators */

	Matrix4 Matrix4::operator*(float value) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] * value;
		return mat;
	}

	Matrix4 Matrix4::operator*=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] *= value;
		return *this;
	}

	Matrix4 Matrix4::operator*(Matrix4 matrix4) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] * matrix4.matrix[i][j];
		return mat;
	}

	Matrix4 Matrix4::operator*=(Matrix4 matrix4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] *= matrix4.matrix[i][j];
		return *this;
	}

	Matrix4 Matrix4::operator*(Vector4& vector) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] * vector.x;
			mat.matrix[i][1] = matrix[i][1] * vector.y;
			mat.matrix[i][2] = matrix[i][2] * vector.z;
			mat.matrix[i][3] = matrix[i][3] * vector.w;
		}
		return mat;
	}

	Matrix4 Matrix4::operator*=(Vector4& vector)
	{
		for (int i = 0; i < 4; i++)
		{
			matrix[i][0] *= vector.x;
			matrix[i][1] *= vector.y;
			matrix[i][2] *= vector.z;
			matrix[i][3] *= vector.w;
		}
		return *this;
	}

	/* Matrix Division Operators */

	Matrix4 Matrix4::operator/(float value) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] / value;
		return mat;
	}

	Matrix4 Matrix4::operator/=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] /= value;
		return *this;
	}

	Matrix4 Matrix4::operator/(Matrix4 matrix4) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] / matrix4.matrix[i][j];
		return mat;
	}

	Matrix4 Matrix4::operator/=(Matrix4 matrix4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] /= matrix4.matrix[i][j];
		return *this;
	}

	Matrix4 Matrix4::operator/(Vector4& vector) const
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] / vector.x;
			mat.matrix[i][1] = matrix[i][1] / vector.y;
			mat.matrix[i][2] = matrix[i][2] / vector.z;
			mat.matrix[i][3] = matrix[i][3] / vector.w;
		}
		return mat;
	}

	Matrix4 Matrix4::operator/=(Vector4& vector)
	{
		for (int i = 0; i < 4; i++)
		{
			matrix[i][0] /= vector.x;
			matrix[i][1] /= vector.y;
			matrix[i][2] /= vector.z;
			matrix[i][3] /= vector.w;
		}
		return *this;
	}
}