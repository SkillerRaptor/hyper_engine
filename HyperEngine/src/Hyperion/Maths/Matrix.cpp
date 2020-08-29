#include "Matrix.h"

namespace Hyperion {

	#define PI 3.141592653589793238462643383279502884L

	//Matrix2x2 Matrix::scale(Matrix2x2 matrix, Vector2 vector)
	//{
	//	Matrix2x2 mat(matrix);
	//	mat.matrix[0][0] = matrix.matrix[0][0] * vector.x;
	//	mat.matrix[1][1] = matrix.matrix[1][1] * vector.y;
	//	return mat;
	//}

	//Matrix3x3 Matrix::scale(Matrix3x3 matrix, Vector3 vector)
	//{
	//	Matrix3x3 mat(matrix);
	//	mat.matrix[0][0] = matrix.matrix[0][0] * vector.x;
	//	mat.matrix[1][1] = matrix.matrix[1][1] * vector.y;
	//	mat.matrix[2][2] = matrix.matrix[2][2] * vector.z;
	//	return mat;
	//}

	Matrix4x4 Matrix::scale(Matrix4x4 matrix, Vector3 vector)
	{
		Matrix4x4 mat(matrix);
		mat.matrix[0][0] = matrix.matrix[0][0] * vector.x;
		mat.matrix[1][1] = matrix.matrix[1][1] * vector.y;
		mat.matrix[2][2] = matrix.matrix[2][2] * vector.z;
		return mat;
	}


	//Matrix2x2 Matrix::translate(Matrix2x2 matrix, Vector2 vector)
	//{
	//	Matrix2x2 mat(matrix);
	//	mat.matrix[0][3] = vector.x;
	//	mat.matrix[1][3] = vector.y;
	//	return mat;
	//}

	//Matrix3x3 Matrix::translate(Matrix3x3 matrix, Vector3 vector)
	//{
	//	Matrix3x3 mat(matrix);
	//	mat.matrix[0][3] = vector.x;
	//	mat.matrix[1][3] = vector.y;
	//	mat.matrix[2][3] = vector.z;
	//	return mat;
	//}

	Matrix4x4 Matrix::translate(Matrix4x4 matrix, Vector3 vector)
	{
		Matrix4x4 mat(matrix);
		mat.matrix[0][3] = vector.x;
		mat.matrix[1][3] = vector.y;
		mat.matrix[2][3] = vector.z;
		return mat;
	}


	//Matrix2x2 Matrix::rotateX(Matrix2x2 matrix, float degrees)
	//{
	//	float angle = (float) Radians(degrees);
	//	Matrix2x2 mat(matrix);
	//	mat.matrix[0][0] = cos(angle);
	//	mat.matrix[1][0] = sin(angle);
	//	return mat;
	//}

	//Matrix3x3 Matrix::rotateX(Matrix3x3 matrix, float degrees)
	//{
	//	float angle = (float) Radians(degrees);
	//	Matrix3x3 mat(matrix);
	//	mat.matrix[1][1] = cos(angle);
	//	mat.matrix[1][2] = -sin(angle);
	//	mat.matrix[2][1] = sin(angle);
	//	mat.matrix[2][2] = cos(angle);
	//	return mat;
	//}

	Matrix4x4 Matrix::rotateX(Matrix4x4 matrix, float degrees)
	{
		float angle = (float) Radians(degrees);
		Matrix4x4 mat(matrix);
		mat.matrix[1][1] = cos(angle);
		mat.matrix[2][1] = sin(angle);
		mat.matrix[1][2] = -sin(angle);
		mat.matrix[2][2] = cos(angle);
		return mat;
	}


	//Matrix2x2 Matrix::rotateY(Matrix2x2 matrix, float degrees)
	//{
	//	float angle = (float) Radians(degrees);
	//	Matrix2x2 mat(matrix);
	//	mat.matrix[0][1] = -sin(angle);
	//	mat.matrix[1][1] = cos(angle);
	//	return mat;
	//}

	//Matrix3x3 Matrix::rotateY(Matrix3x3 matrix, float degrees)
	//{
	//	float angle = (float) Radians(degrees);
	//	Matrix3x3 mat(matrix);
	//	mat.matrix[0][0] = cos(angle);
	//	mat.matrix[0][2] = sin(angle);
	//	mat.matrix[2][0] = -sin(angle);
	//	mat.matrix[2][2] = cos(angle);
	//	return mat;
	//}

	Matrix4x4 Matrix::rotateY(Matrix4x4 matrix, float degrees)
	{
		float angle = (float) Radians(degrees);
		Matrix4x4 mat(matrix);
		mat.matrix[0][0] = cos(angle);
		mat.matrix[0][2] = sin(angle);
		mat.matrix[2][0] = -sin(angle);
		mat.matrix[2][2] = cos(angle);
		return mat;
	}


	//Matrix3x3 Matrix::rotateZ(Matrix3x3 matrix, float degrees)
	//{
	//	float angle = (float) Radians(degrees);
	//	Matrix3x3 mat(matrix);
	//	mat.matrix[0][0] = cos(angle);
	//	mat.matrix[0][1] = -sin(angle);
	//	mat.matrix[1][0] = sin(angle);
	//	mat.matrix[1][1] = cos(angle);
	//	return mat;
	//}

	Matrix4x4 Matrix::rotateZ(Matrix4x4 matrix, float degrees)
	{
		float angle = (float) Radians(degrees);
		Matrix4x4 mat(matrix);
		mat.matrix[0][0] = cos(angle);
		mat.matrix[0][1] = -sin(angle);
		mat.matrix[1][0] = sin(angle);
		mat.matrix[1][1] = cos(angle);
		return mat;
	}

	Matrix4x4 Matrix::ortho(float left, float right, float bottom, float top, float near, float far)
	{
		Matrix4x4 mat(1.0f);
		if (right == -left && top == -bottom)
		{
			mat.matrix[0][0] = 1 / right;
			mat.matrix[1][1] = 1 / top;
			mat.matrix[2][2] = -2 / (far - near);
			mat.matrix[2][3] = -((far + near) / (far - near));
		}
		else
		{
			mat.matrix[0][0] = 2 / (right - left);
			mat.matrix[0][3] = -((right + left) / (right - left));
			mat.matrix[1][1] = 2 / (top - bottom);
			mat.matrix[1][3] = -((top + bottom) / (top - bottom));
			mat.matrix[2][2] = -2 / (far - near);
			mat.matrix[2][3] = -((far + near) / (far - near));
		}
		return mat;
	}

	Matrix4x4 Matrix::perspective(float degrees, float aspectRatio, float near, float far)
	{
		Matrix4x4 mat(1.0f);
		float angle = (float) Radians(degrees);
		float tanAngle = tan(angle / 2);
		mat.matrix[0][0] = 1 / (aspectRatio * tanAngle);
		mat.matrix[1][1] = 1 / tanAngle;
		mat.matrix[2][2] = -((far + near) / (far - near));
		mat.matrix[2][3] = -((2 * far * near) / (far - near));
		mat.matrix[3][2] = -1;
		return mat;
	}
}