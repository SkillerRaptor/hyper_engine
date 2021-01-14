#pragma once

#include "HyperMath/Vector.hpp"
#include "HyperMath/Matrix.hpp"

namespace Hyperion
{
	class Quaternion
	{
	private:
		float m_Scalar;
		Vec3 m_Axis;

	public:
		explicit Quaternion(Vec3 rotation);
		Quaternion(float scalar, Vec3 axis);
		~Quaternion();

		Quaternion(const Quaternion& other);
		Quaternion& operator=(const Quaternion& other);

		Quaternion operator+(const Quaternion& other) const;
		void operator+=(const Quaternion& other);

		Quaternion operator-(const Quaternion& other) const;
		void operator-=(const Quaternion& other);

		Quaternion operator*(const Quaternion& other) const;
		void operator*=(const Quaternion& other);

		Quaternion operator*(const float value) const;
		void operator*=(const float value);

	public:
		static float Norm(Quaternion quaternion);
		static Quaternion Normalize(Quaternion quaternion);
		static Quaternion ConvertToUnitNorm(Quaternion quaternion);
		static Quaternion Conjugate(Quaternion quaternion);
		static Quaternion Inverse(Quaternion quaternion);

		static Quaternion RotateQuaternion(float angle, Vec3 axis);

		static Vec3 RotateVector(Vec3 vector, float angle, Vec3 axis);

		static Mat4 ConvertToMatrix(Vec4 vector);
		static Mat4 ConvertToMatrix(Quaternion quaternion);
	};
}
