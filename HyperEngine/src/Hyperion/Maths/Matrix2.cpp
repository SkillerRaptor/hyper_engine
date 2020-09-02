#include "Matrix2.hpp"

namespace Hyperion
{
	Matrix2::Matrix2()
		: Matrix2(0.0f)
	{
	}

	Matrix2::Matrix2(float value)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				matrix[i][j] = 0.0f;
				if (i == j)
					matrix[i][j] = value;
			}
	}

	Matrix2::Matrix2(const Matrix2& matrix2)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] = matrix2.matrix[i][j];
	}

	/* Matrix Addition Operators */

	Matrix2 Matrix2::operator+(float value) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] + value;
		return mat;
	}

	Matrix2 Matrix2::operator+=(float value)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] += value;
		return *this;
	}

	Matrix2 Matrix2::operator+(const Matrix2& matrix2) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] + matrix2.matrix[i][j];
		return mat;
	}

	Matrix2 Matrix2::operator+=(const Matrix2& matrix2)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] += matrix2.matrix[i][j];
		return *this;
	}

	Matrix2 Matrix2::operator+(const Vector2& vector) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
		{
			mat.matrix[i][0] = matrix[i][0] + vector.x;
			mat.matrix[i][1] = matrix[i][1] + vector.y;
		}
		return mat;
	}

	Matrix2 Matrix2::operator+=(const Vector2& vector)
	{
		for (int i = 0; i < 2; i++)
		{
			matrix[i][0] += vector.x;
			matrix[i][1] += vector.y;
		}
		return *this;
	}

	/* Matrix Subtraction Operators */

	Matrix2 Matrix2::operator-(float value) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] - value;
		return mat;
	}

	Matrix2 Matrix2::operator-=(float value)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] -= value;
		return *this;
	}

	Matrix2 Matrix2::operator-(const Matrix2& matrix2) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] - matrix2.matrix[i][j];
		return mat;
	}

	Matrix2 Matrix2::operator-=(const Matrix2& matrix2)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] -= matrix2.matrix[i][j];
		return *this;
	}

	Matrix2 Matrix2::operator-(const Vector2& vector) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
		{
			mat.matrix[i][0] = matrix[i][0] - vector.x;
			mat.matrix[i][1] = matrix[i][1] - vector.y;
		}
		return mat;
	}

	Matrix2 Matrix2::operator-=(const Vector2& vector)
	{
		for (int i = 0; i < 2; i++)
		{
			matrix[i][0] -= vector.x;
			matrix[i][1] -= vector.y;
		}
		return *this;
	}

	/* Matrix Multiplication Operators */

	Matrix2 Matrix2::operator*(float value) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] * value;
		return mat;
	}

	Matrix2 Matrix2::operator*=(float value)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] *= value;
		return *this;
	}

	Matrix2 Matrix2::operator*(const Matrix2& matrix2) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] * matrix2.matrix[i][j];
		return mat;
	}

	Matrix2 Matrix2::operator*=(const Matrix2& matrix2)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] *= matrix2.matrix[i][j];
		return *this;
	}

	Matrix2 Matrix2::operator*(const Vector2& vector) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
		{
			mat.matrix[i][0] = matrix[i][0] * vector.x;
			mat.matrix[i][1] = matrix[i][1] * vector.y;
		}
		return mat;
	}

	Matrix2 Matrix2::operator*=(const Vector2& vector)
	{
		for (int i = 0; i < 2; i++)
		{
			matrix[i][0] *= vector.x;
			matrix[i][1] *= vector.y;
		}
		return *this;
	}

	/* Matrix Division Operators */

	Matrix2 Matrix2::operator/(float value) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] / value;
		return mat;
	}

	Matrix2 Matrix2::operator/=(float value)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] /= value;
		return *this;
	}

	Matrix2 Matrix2::operator/(const Matrix2& matrix2) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				mat.matrix[i][j] = matrix[i][j] / matrix2.matrix[i][j];
		return mat;
	}

	Matrix2 Matrix2::operator/=(const Matrix2& matrix2)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				matrix[i][j] /= matrix2.matrix[i][j];
		return *this;
	}

	Matrix2 Matrix2::operator/(const Vector2& vector) const
	{
		Matrix2 mat;
		for (int i = 0; i < 2; i++)
		{
			mat.matrix[i][0] = matrix[i][0] / vector.x;
			mat.matrix[i][1] = matrix[i][1] / vector.y;
		}
		return mat;
	}

	Matrix2 Matrix2::operator/=(const Vector2& vector)
	{
		for (int i = 0; i < 2; i++)
		{
			matrix[i][0] /= vector.x;
			matrix[i][1] /= vector.y;
		}
		return *this;
	}

	bool Matrix2::operator==(const Matrix2& matrix2)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				if (matrix[i][j] != matrix2.matrix[i][j])
					return false;
		return true;
	}

	bool Matrix2::operator!=(const Matrix2& matrix2)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				if (matrix[i][j] != matrix2.matrix[i][j])
					return true;
		return false;
	}
}