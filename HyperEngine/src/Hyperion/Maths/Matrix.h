#pragma once

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace Hyperion 
{
	class Matrix
	{
		//static Matrix2x2 scale(Matrix2x2 matrix, Vector2 vector);
		//static Matrix3x3 scale(Matrix3x3 matrix, Vector3 vector);
		static Matrix4x4 scale(Matrix4x4 matrix, Vector3 vector);

		//static Matrix2x2 translate(Matrix2x2 matrix, Vector2 vector);
		//static Matrix3x3 translate(Matrix3x3 matrix, Vector3 vector);
		static Matrix4x4 translate(Matrix4x4 matrix, Vector3 vector);

		//static Matrix2x2 rotateX(Matrix2x2 matrix, float degrees);
		//static Matrix3x3 rotateX(Matrix3x3 matrix, float degrees);
		static Matrix4x4 rotateX(Matrix4x4 matrix, float degrees);

		//static Matrix2x2 rotateY(Matrix2x2 matrix, float degrees);
		//static Matrix3x3 rotateY(Matrix3x3 matrix, float degrees);
		static Matrix4x4 rotateY(Matrix4x4 matrix, float degreesx2);

		//static Matrix3x3 rotateZ(Matrix3x3 matrix, float degrees);
		static Matrix4x4 rotateZ(Matrix4x4 matrix, float degrees);

		static Matrix4x4 ortho(float left, float right, float bottom, float top, float near, float far);
		static Matrix4x4 perspective(float degrees, float aspectRatio, float near, float far);
	};
}
