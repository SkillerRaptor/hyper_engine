#pragma once

#include "HyperMath/Matrix2.hpp"
#include "HyperMath/Matrix3.hpp"
#include "HyperMath/Matrix4.hpp"

namespace HyperMath
{
	typedef Matrix2<signed char> ByteMat2;
	typedef Matrix3<signed char> ByteMat3;
	typedef Matrix4<signed char> ByteMat4;
	typedef Matrix2<unsigned char> UByteMat2;
	typedef Matrix3<unsigned char> UByteMat3;
	typedef Matrix4<unsigned char> UByteMat4;
	typedef Matrix2<short> ShortMat2;
	typedef Matrix3<short> ShortMat3;
	typedef Matrix4<short> ShortMat4;
	typedef Matrix2<unsigned short> UShortMat2;
	typedef Matrix3<unsigned short> UShortMat3;
	typedef Matrix4<unsigned short> UShortMat4;
	typedef Matrix2<int> IntMat2;
	typedef Matrix3<int> IntMat3;
	typedef Matrix4<int> IntMat4;
	typedef Matrix2<unsigned int> UIntMat2;
	typedef Matrix3<unsigned int> UIntMat3;
	typedef Matrix4<unsigned int> UIntMat4;
	typedef Matrix2<long> LongMat2;
	typedef Matrix3<long> LongMat3;
	typedef Matrix4<long> LongMat4;
	typedef Matrix2<unsigned long> ULongMat2;
	typedef Matrix3<unsigned long> ULongMat3;
	typedef Matrix4<unsigned long> ULongMat4;
	typedef Matrix2<float> Mat2;
	typedef Matrix3<float> Mat3;
	typedef Matrix4<float> Mat4;
	typedef Matrix2<double> DoubleMat2;
	typedef Matrix3<double> DoubleMat3;
	typedef Matrix4<double> DoubleMat4;
	typedef Matrix2<long double> LDoubleMat2;
	typedef Matrix3<long double> LDoubleMat3;
	typedef Matrix4<long double> LDoubleMat4;

	namespace Matrix
	{
		struct Transform
		{
			Vec3 Position;
			Vec3 Rotation;
			Vec3 Scale;
		};

		Transform GetTransformFromMatrix(Mat4 matrix);

		Mat2 Inverse(Mat2 matrix);
		Mat3 Inverse(Mat3 matrix);
		Mat4 Inverse(Mat4 matrix);

		Mat4 Scale(Mat4 matrix, Vec3 vector);
		Mat4 Translate(Mat4 matrix, Vec3 vector);

		Mat4 LookAt(Vec3 eye, Vec3 center, Vec3 up);
		Mat4 Ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		Mat4 Perspective(float fov, float aspectRatio, float nearPlane, float farPlane);
	};
}
