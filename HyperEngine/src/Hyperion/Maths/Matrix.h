#pragma once

#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace Hyperion 
{
	class Matrix
	{
		static Matrix4 Scale(Matrix4 matrix, Vector3 vector);
		static Matrix4 Translate(Matrix4 matrix, Vector3 vector);

		static Matrix4 RotateX(Matrix4 matrix, float degrees);
		static Matrix4 RotateY(Matrix4 matrix, float degreesx2);
		static Matrix4 RotateZ(Matrix4 matrix, float degrees);

		static Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far);
		static Matrix4 Perspective(float degrees, float aspectRatio, float near, float far);
	};
}
