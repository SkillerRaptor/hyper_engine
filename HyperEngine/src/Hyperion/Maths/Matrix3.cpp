#include "Matrix3.h"

namespace Hyperion 
{
	Matrix3::Matrix3()
		: Matrix3(0.0f)
	{
	}

	Matrix3::Matrix3(float value)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				matrix[i][j] = 0.0f;
				if (i == j)
					matrix[i][j] = value;
			}
	}

	Matrix3::Matrix3(const Matrix3& matrix3)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] = matrix3.matrix[i][j];
	}

	/* Matrix Addition Operators */

	Matrix3 Matrix3::operator+(float value) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] + value;
		return mat;
	}

	Matrix3 Matrix3::operator+=(float value)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] += value;
		return *this;
	}

	Matrix3 Matrix3::operator+(Matrix3 matrix3) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] + matrix3.matrix[i][j];
		return mat;
	}

	Matrix3 Matrix3::operator+=(Matrix3 matrix3)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] += matrix3.matrix[i][j];
		return *this;
	}

	Matrix3 Matrix3::operator+(Vector3& vector) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
		{
			mat.matrix[i][0] = matrix[i][0] + vector.x;
			mat.matrix[i][1] = matrix[i][1] + vector.y;
			mat.matrix[i][2] = matrix[i][2] + vector.z;
		}
		return mat;
	}

	Matrix3 Matrix3::operator+=(Vector3& vector)
	{
		for (int i = 0; i < 3; i++)
		{
			matrix[i][0] += vector.x;
			matrix[i][1] += vector.y;
			matrix[i][2] += vector.z;
		}
		return *this;
	}

	/* Matrix Subtraction Operators */

	Matrix3 Matrix3::operator-(float value) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] - value;
		return mat;
	}

	Matrix3 Matrix3::operator-=(float value)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] -= value;
		return *this;
	}

	Matrix3 Matrix3::operator-(Matrix3 matrix3) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] - matrix3.matrix[i][j];
		return mat;
	}

	Matrix3 Matrix3::operator-=(Matrix3 matrix3)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] -= matrix3.matrix[i][j];
		return *this;
	}

	Matrix3 Matrix3::operator-(Vector3& vector) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
		{
			mat.matrix[i][0] = matrix[i][0] - vector.x;
			mat.matrix[i][1] = matrix[i][1] - vector.y;
			mat.matrix[i][2] = matrix[i][2] - vector.z;
		}
		return mat;
	}

	Matrix3 Matrix3::operator-=(Vector3& vector)
	{
		for (int i = 0; i < 3; i++)
		{
			matrix[i][0] -= vector.x;
			matrix[i][1] -= vector.y;
			matrix[i][2] -= vector.z;
		}
		return *this;
	}

	/* Matrix Multiplication Operators */

	Matrix3 Matrix3::operator*(float value) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] * value;
		return mat;
	}

	Matrix3 Matrix3::operator*=(float value)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] *= value;
		return *this;
	}

	Matrix3 Matrix3::operator*(Matrix3 matrix3) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] * matrix3.matrix[i][j];
		return mat;
	}

	Matrix3 Matrix3::operator*=(Matrix3 matrix3)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] *= matrix3.matrix[i][j];
		return *this;
	}

	Matrix3 Matrix3::operator*(Vector3& vector) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
		{
			mat.matrix[i][0] = matrix[i][0] * vector.x;
			mat.matrix[i][1] = matrix[i][1] * vector.y;
			mat.matrix[i][2] = matrix[i][2] * vector.z;
		}
		return mat;
	}

	Matrix3 Matrix3::operator*=(Vector3& vector)
	{
		for (int i = 0; i < 3; i++)
		{
			matrix[i][0] *= vector.x;
			matrix[i][1] *= vector.y;
			matrix[i][2] *= vector.z;
		}
		return *this;
	}

	/* Matrix Division Operators */

	Matrix3 Matrix3::operator/(float value) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] / value;
		return mat;
	}

	Matrix3 Matrix3::operator/=(float value)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] /= value;
		return *this;
	}

	Matrix3 Matrix3::operator/(Matrix3 matrix3) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.matrix[i][j] = matrix[i][j] / matrix3.matrix[i][j];
		return mat;
	}

	Matrix3 Matrix3::operator/=(Matrix3 matrix3)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] /= matrix3.matrix[i][j];
		return *this;
	}

	Matrix3 Matrix3::operator/(Vector3& vector) const
	{
		Matrix3 mat;
		for (int i = 0; i < 3; i++)
		{
			mat.matrix[i][0] = matrix[i][0] / vector.x;
			mat.matrix[i][1] = matrix[i][1] / vector.y;
			mat.matrix[i][2] = matrix[i][2] / vector.z;
		}
		return mat;
	}

	Matrix3 Matrix3::operator/=(Vector3& vector)
	{
		for (int i = 0; i < 3; i++)
		{
			matrix[i][0] /= vector.x;
			matrix[i][1] /= vector.y;
			matrix[i][2] /= vector.z;
		}
		return *this;
	}
}