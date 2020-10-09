#include "Matrix.hpp"

namespace Hyperion
{
	namespace Matrix
	{
		Transform GetTransformFromMatrix(Matrix4<float> matrix)
		{
			Transform transform;
			transform.Position = Vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
			transform.Rotation = Vec3(0.0f, 0.0f, 0.0f);
			transform.Scale = Vec3(Vec3(matrix[0][0], matrix[1][0], matrix[2][0]).Magnitude(), Vec3(matrix[0][1], matrix[1][1], matrix[2][1]).Magnitude(), Vec3(matrix[0][2], matrix[1][1], matrix[2][2]).Magnitude());
			return transform;
		}

		Matrix4<float> Scale(Matrix4<float> matrix, Vector3<float> vector)
		{
			Matrix4<float> mat(matrix);
			mat[0][0] = matrix[0][0] * vector.x;
			mat[1][1] = matrix[1][1] * vector.y;
			mat[2][2] = matrix[2][2] * vector.z;
			return mat;
		}

		Matrix4<float> Translate(Matrix4<float> matrix, Vector3<float> vector)
		{
			Matrix4<float> mat(matrix);
			mat[0][3] = vector.x;
			mat[1][3] = vector.y;
			mat[2][3] = vector.z;
			return mat;
		}

		Matrix4<float> Rotate(Matrix4<float> matrix, Vector3<float> degrees)
		{
			Matrix4<float> mat(matrix);
			return mat;
		}

		Matrix4<float> RotateX(Matrix4<float> matrix, float degrees)
		{
			float angle = (float)degrees * ((float)M_PI) / 180;
			Matrix4<float> mat(matrix);
			mat[1][1] = cos(angle);
			mat[1][2] = -sin(angle);
			mat[2][1] = sin(angle);
			mat[2][2] = cos(angle);
			return mat;
		}

		Matrix4<float> RotateY(Matrix4<float> matrix, float degrees)
		{
			float angle = (float)degrees * ((float)M_PI) / 180;
			Matrix4<float> mat(matrix);
			mat[0][0] = cos(angle);
			mat[0][2] = sin(angle);
			mat[2][0] = -sin(angle);
			mat[2][2] = cos(angle);
			return mat;
		}

		Matrix4<float> RotateZ(Matrix4<float> matrix, float degrees)
		{
			float angle = (float)degrees * ((float)M_PI) / 180;
			Matrix4<float> mat(matrix);
			mat[0][0] = cos(angle);
			mat[0][1] = -sin(angle);
			mat[1][0] = sin(angle);
			mat[1][1] = cos(angle);
			return mat;
		}

		Matrix4<float> Ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane)
		{
			Matrix4<float> matrix(1.0f);

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

		Matrix4<float> Perspective(float degrees, float aspectRatio, float nearPlane, float farPlane)
		{
			Matrix4<float> matrix(1.0f);
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
