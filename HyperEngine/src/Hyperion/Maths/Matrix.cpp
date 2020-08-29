#include "Matrix.h"

namespace Hyperion {

	Matrix4 Matrix::Scale(Matrix4 matrix, Vector3 vector)
	{
		Matrix4 mat(matrix);
		mat[0][0] = matrix[0][0] * vector.x;
		mat[1][1] = matrix[1][1] * vector.y;
		mat[2][2] = matrix[2][2] * vector.z;
		return mat;
	}

	Matrix4 Matrix::Translate(Matrix4 matrix, Vector3 vector)
	{
		Matrix4 mat(matrix);
		mat[0][3] = vector.x;
		mat[1][3] = vector.y;
		mat[2][3] = vector.z;
		return mat;
	}

	Matrix4 Matrix::RotateX(Matrix4 matrix, float degrees)
	{
		float angle = (float) degrees * ((float) M_PI) / 180;
		Matrix4 mat(matrix);
		mat[1][1] = cos(angle);
		mat[2][1] = sin(angle);
		mat[1][2] = -sin(angle);
		mat[2][2] = cos(angle);
		return mat;
	}

	Matrix4 Matrix::RotateY(Matrix4 matrix, float degrees)
	{
		float angle = (float) degrees * ((float) M_PI) / 180;
		Matrix4 mat(matrix);
		mat[0][0] = cos(angle);
		mat[0][2] = sin(angle);
		mat[2][0] = -sin(angle);
		mat[2][2] = cos(angle);
		return mat;
	}

	Matrix4 Matrix::RotateZ(Matrix4 matrix, float degrees)
	{
		float angle = (float) degrees * ((float) M_PI) / 180;
		Matrix4 mat(matrix);
		mat[0][0] = cos(angle);
		mat[0][1] = -sin(angle);
		mat[1][0] = sin(angle);
		mat[1][1] = cos(angle);
		return mat;
	}

	Matrix4 Matrix::Ortho(float left, float right, float bottom, float top, float near, float far)
	{
		Matrix4 matrix(1.0f);

		if (right == -left && top == -bottom)
		{
			matrix[0][0] = 1 / right;
			matrix[1][1] = 1 / top;
			matrix[2][2] = -2 / (far - near);
			matrix[2][3] = -((far + near) / (far - near));
		}
		else
		{
			matrix[0][0] = 2 / (right - left);
			matrix[0][3] = -((right + left) / (right - left));
			matrix[1][1] = 2 / (top - bottom);
			matrix[1][3] = -((top + bottom) / (top - bottom));
			matrix[2][2] = -2 / (far - near);
			matrix[2][3] = -((far + near) / (far - near));
		}
		return matrix;
	}

	Matrix4 Matrix::Perspective(float degrees, float aspectRatio, float near, float far)
	{
		Matrix4 matrix(1.0f);
		float angle = (float) degrees * ((float) M_PI) / 180;
		float tanAngle = tan(angle / 2);
		matrix[0][0] = 1 / (aspectRatio * tanAngle);
		matrix[1][1] = 1 / tanAngle;
		matrix[2][2] = -((far + near) / (far - near));
		matrix[2][3] = -((2 * far * near) / (far - near));
		matrix[3][2] = -1;
		return matrix;
	}
}