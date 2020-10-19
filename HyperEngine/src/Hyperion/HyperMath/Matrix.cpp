#include "Matrix.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Hyperion
{
	namespace Matrix
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

		Mat4 Perspective(float degrees, float aspectRatio, float nearPlane, float farPlane)
		{
			Mat4 matrix(1.0f);
			float angle = (float)degrees * ((float)M_PI) / 180;
			float tanAngle = tan(angle / 2);
			matrix[0][0] = 1 / (aspectRatio * tanAngle);
			matrix[1][1] = 1 / tanAngle;
			matrix[2][2] = (-(farPlane + nearPlane)) / (farPlane - nearPlane);
			matrix[2][3] = (-2 * farPlane * nearPlane) / (farPlane - nearPlane);
			matrix[3][2] = -1;
			return matrix;
		}
	}
}
