#include "Matrix4x4.h"

namespace Hyperion {

	Matrix4x4::Matrix4x4()
		: Matrix4x4(0.0f)
	{
	}

	Matrix4x4::Matrix4x4(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				matrix[i][j] = 0;
				if (i == j)
					matrix[i][j] = value;
			}
	}

	Matrix4x4::Matrix4x4(const Matrix4x4& matrix4x4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] = matrix4x4.matrix[i][j];
	}

	Matrix4x4 Matrix4x4::operator+(float value) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] + value;
		return mat;
	}

	Matrix4x4 Matrix4x4::operator+(Matrix4x4 matrix4x4) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] + matrix4x4.matrix[i][j];
		return mat;
	}

	Matrix4x4 Matrix4x4::operator-(float value) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] - value;
		return mat;
	}

	Matrix4x4 Matrix4x4::operator-(Matrix4x4 matrix4x4) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] - matrix4x4.matrix[i][j];
		return mat;
	}

	Matrix4x4 Matrix4x4::operator*(float value) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] * value;
		return mat;
	}

	Matrix4x4 Matrix4x4::operator*(Matrix4x4 matrix4x4) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] * matrix4x4.matrix[i][j];
		return mat;
	}

	Matrix4x4 Matrix4x4::operator/(float value) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] / value;
		return mat;
	}

	Matrix4x4 Matrix4x4::operator/(Matrix4x4 matrix4x4) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.matrix[i][j] = matrix[i][j] / matrix4x4.matrix[i][j];
		return mat;
	}

	Matrix4x4 Matrix4x4::operator+=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] += value;
		return *this;
	}

	Matrix4x4 Matrix4x4::operator+=(Matrix4x4 matrix4x4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] += matrix4x4.matrix[i][j];
		return *this;
	}

	Matrix4x4 Matrix4x4::operator-=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] -= value;
		return *this;
	}

	Matrix4x4 Matrix4x4::operator-=(Matrix4x4 matrix4x4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] -= matrix4x4.matrix[i][j];
		return *this;
	}

	Matrix4x4 Matrix4x4::operator*=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] *= value;
		return *this;
	}

	Matrix4x4 Matrix4x4::operator*=(Matrix4x4 matrix4x4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] *= matrix4x4.matrix[i][j];
		return *this;
	}

	Matrix4x4 Matrix4x4::operator/=(float value)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] /= value;
		return *this;
	}

	Matrix4x4 Matrix4x4::operator/=(Matrix4x4 matrix4x4)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				matrix[i][j] /= matrix4x4.matrix[i][j];
		return *this;
	}

	Matrix4x4 Matrix4x4::operator+(Vector4& vector) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] + vector.x;
			mat.matrix[i][1] = matrix[i][1] + vector.y;
			mat.matrix[i][2] = matrix[i][2] + vector.z;
			mat.matrix[i][3] = matrix[i][3] + vector.w;
		}
		return mat;
	}

	Matrix4x4 Matrix4x4::operator+=(Vector4& vector)
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

	Matrix4x4 Matrix4x4::operator-(Vector4& vector) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] - vector.x;
			mat.matrix[i][1] = matrix[i][1] - vector.y;
			mat.matrix[i][2] = matrix[i][2] - vector.z;
			mat.matrix[i][3] = matrix[i][3] - vector.w;
		}
		return mat;
	}

	Matrix4x4 Matrix4x4::operator-=(Vector4& vector)
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

	Matrix4x4 Matrix4x4::operator*(Vector4& vector) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] * vector.x;
			mat.matrix[i][1] = matrix[i][1] * vector.y;
			mat.matrix[i][2] = matrix[i][2] * vector.z;
			mat.matrix[i][3] = matrix[i][3] * vector.w;
		}
		return mat;
	}

	Matrix4x4 Matrix4x4::operator*=(Vector4& vector)
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

	Matrix4x4 Matrix4x4::operator/(Vector4& vector) const
	{
		Matrix4x4 mat;
		for (int i = 0; i < 4; i++)
		{
			mat.matrix[i][0] = matrix[i][0] / vector.x;
			mat.matrix[i][1] = matrix[i][1] / vector.y;
			mat.matrix[i][2] = matrix[i][2] / vector.z;
			mat.matrix[i][3] = matrix[i][3] / vector.w;
		}
		return mat;
	}

	Matrix4x4 Matrix4x4::operator/=(Vector4& vector)
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