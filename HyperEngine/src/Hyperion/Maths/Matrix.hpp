#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "Matrix2.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"

namespace Hyperion 
{
	namespace Matrix
	{
		template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		Matrix4<T> Scale(Matrix4<T> matrix, Vector3<T> vector)
		{
			Matrix4<T> mat(matrix);
			mat[0][0] = matrix[0][0] * vector.x;
			mat[1][1] = matrix[1][1] * vector.y;
			mat[2][2] = matrix[2][2] * vector.z;
			return mat;
		}

		template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		Matrix4<T> Translate(Matrix4<T> matrix, Vector3<T> vector)
		{
			Matrix4<T> mat(matrix);
			mat[0][3] = vector.x;
			mat[1][3] = vector.y;
			mat[2][3] = vector.z;
			return mat;
		}

		template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		Matrix4<T> RotateX(Matrix4<T> matrix, float degrees)
		{
			T angle = (T) degrees * ((T) M_PI) / 180;
			Matrix4<T> mat(matrix);
			mat[1][1] = cos(angle);
			mat[2][1] = sin(angle);
			mat[1][2] = -sin(angle);
			mat[2][2] = cos(angle);
			return mat;
		}

		template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		Matrix4<T> RotateY(Matrix4<T> matrix, float degrees)
		{
			T angle = (T) degrees * ((T) M_PI) / 180;
			Matrix4<T> mat(matrix);
			mat[0][0] = cos(angle);
			mat[0][2] = sin(angle);
			mat[2][0] = -sin(angle);
			mat[2][2] = cos(angle);
			return mat;
		}

		template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		Matrix4<T> RotateZ(Matrix4<T> matrix, T degrees)
		{
			T angle = (T) degrees * ((T) M_PI) / 180;
			Matrix4<T> mat(matrix);
			mat[0][0] = cos(angle);
			mat[0][1] = -sin(angle);
			mat[1][0] = sin(angle);
			mat[1][1] = cos(angle);
			return mat;
		}

		template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		Matrix4<T> Ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane)
		{
			Matrix4<T> matrix(1.0f);

			if (right == -left && top == -bottom)
			{
				matrix[0][0] = 1 / right;
				matrix[1][1] = 1 / top;
				matrix[2][2] = -2 / (farPlane - nearPlane);
				matrix[2][3] = -((farPlane + nearPlane) / (farPlane - nearPlane));
			}
			else
			{
				matrix[0][0] = 2 / (right - left);
				matrix[0][3] = -((right + left) / (right - left));
				matrix[1][1] = 2 / (top - bottom);
				matrix[1][3] = -((top + bottom) / (top - bottom));
				matrix[2][2] = -2 / (farPlane - nearPlane);
				matrix[2][3] = -((farPlane + nearPlane) / (farPlane - nearPlane));
			}
			return matrix;
		}

		template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		Matrix4<T> Perspective(float degrees, float aspectRatio, float nearPlane, float farPlane)
		{
			Matrix4<T> matrix(1.0f);
			float angle = (float)degrees * ((float)M_PI) / 180;
			float tanAngle = tan(angle / 2);
			matrix[0][0] = 1 / (aspectRatio * tanAngle);
			matrix[1][1] = 1 / tanAngle;
			matrix[2][2] = -((farPlane + nearPlane) / (farPlane - nearPlane));
			matrix[2][3] = -((2 * farPlane * nearPlane) / (farPlane - nearPlane));
			matrix[3][2] = -1;
			return matrix;
		}
	};
}
