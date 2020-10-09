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
		struct Transform
		{
			Vec3 Position;
			Vec3 Rotation;
			Vec3 Scale;
		};

		Transform GetTransformFromMatrix(Matrix4<float> matrix);

		Matrix4<float> Scale(Matrix4<float> matrix, Vector3<float> vector);
		Matrix4<float> Translate(Matrix4<float> matrix, Vector3<float> vector);

		Matrix4<float> Rotate(Matrix4<float> matrix, Vector3<float> degrees);
		Matrix4<float> RotateX(Matrix4<float> matrix, float degrees);
		Matrix4<float> RotateY(Matrix4<float> matrix, float degrees);
		Matrix4<float> RotateZ(Matrix4<float> matrix, float degrees);

		Matrix4<float> Ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		Matrix4<float> Perspective(float degrees, float aspectRatio, float nearPlane, float farPlane);
	};
}
