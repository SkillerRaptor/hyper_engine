#include "HyperMath/Matrix.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace HyperMath::Matrix
{
	Transform GetTransformFromMatrix(Mat4 matrix)
	{
		Transform transform;
		transform.Position = Vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
		transform.Rotation = Vec3(0.0f, 0.0f, 0.0f);
		transform.Scale = Vec3(Vec3(matrix[0][0], matrix[1][0], matrix[2][0]).Magnitude(), Vec3(matrix[0][1], matrix[1][1], matrix[2][1]).Magnitude(), Vec3(matrix[0][2], matrix[1][1], matrix[2][2]).Magnitude());
		return transform;
	}

	Mat2 Inverse(Mat2 matrix)
	{
		Mat2 mat;
		for (size_t i = 0; i < 2; i++)
			for (size_t j = 0; j < 2; j++)
				mat[i][j] = matrix[j][i];
		return mat;
	}

	Mat3 Inverse(Mat3 matrix)
	{
		Mat3 mat;
		for (size_t i = 0; i < 3; i++)
			for (size_t j = 0; j < 3; j++)
				mat[i][j] = matrix[j][i];
		return mat;
	}

	Mat4 Inverse(Mat4 matrix)
	{
		Mat4 mat;
		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++)
				mat[i][j] = matrix[j][i];
		return mat;
	}

	Mat4 Scale(Mat4 matrix, Vec3 vector)
	{
		Mat4 mat(matrix);
		mat[0][0] = matrix[0][0] * vector.x;
		mat[1][1] = matrix[1][1] * vector.y;
		mat[2][2] = matrix[2][2] * vector.z;
		return mat;
	}

	Mat4 Translate(Mat4 matrix, Vec3 vector)
	{
		Mat4 mat(matrix);
		mat[0][3] = vector.x;
		mat[1][3] = vector.y;
		mat[2][3] = vector.z;
		return mat;
	}

	Mat4 LookAt(Vec3 eye, Vec3 center, Vec3 up)
	{
		Mat4 matrix(1.0f);

		Vec3 forward = Vector::Normalize(center - eye);
		Vec3 right = Vector::Normalize(Vector::Cross(forward, up));
		Vec3 upDir = Vector::Cross(right, forward);

		matrix[0][0] = right.x;
		matrix[0][1] = right.y;
		matrix[0][2] = right.z;
		matrix[0][3] = -Vector::Dot(right, eye);

		matrix[1][0] = upDir.x;
		matrix[1][1] = upDir.y;
		matrix[1][2] = upDir.z;
		matrix[1][3] = -Vector::Dot(upDir, eye);

		matrix[2][0] = -forward.x;
		matrix[2][1] = -forward.y;
		matrix[2][2] = -forward.z;
		matrix[2][3] = Vector::Dot(forward, eye);

 		return matrix;
	}

	Mat4 Ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane)
	{
		Mat4 matrix(1.0f);

		if (right == -left && top == -bottom)
		{
			matrix[0][0] = 1 / right;
			matrix[1][1] = 1 / top;
			matrix[2][2] = -2 / (farPlane - nearPlane);
			matrix[3][2] = -((farPlane + nearPlane) / (farPlane - nearPlane));
		}
		else
		{
			matrix[0][0] = 2 / (right - left);
			matrix[1][1] = 2 / (top - bottom);
			matrix[2][2] = -2 / (farPlane - nearPlane);
			matrix[3][0] = -((right + left) / (right - left));
			matrix[3][1] = -((top + bottom) / (top - bottom));
			matrix[3][2] = -((farPlane + nearPlane) / (farPlane - nearPlane));
		}
		return matrix;
	}

	Mat4 Perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		Mat4 matrix(0.0f);
		float angle = (float)fov * ((float)M_PI) / 180;
		float tanAngle = tan(angle / 2.0f);
		matrix[0][0] = 1.0f / (aspectRatio * tanAngle);
		matrix[1][1] = 1.0f / tanAngle;
		matrix[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
		matrix[2][3] = -(2 * farPlane * nearPlane) / (farPlane - nearPlane);
		matrix[3][2] = -1.0f;

		return matrix;
	}
}
