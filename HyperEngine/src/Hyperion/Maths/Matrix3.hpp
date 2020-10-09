#pragma once

#include <cmath>
#include <iostream>

#include "Vector.hpp"

namespace Hyperion 
{
	template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	class Matrix3
	{
	public:
		T matrix[3][3];

	public:
		Matrix3()
			: Matrix3(0.0f)
		{
		}

		Matrix3(T value)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
				{
					matrix[i][j] = 0.0f;
					if (i == j)
						matrix[i][j] = value;
				}
		}

		Matrix3(const Matrix3<T>& matrix3)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] = matrix3.matrix[i][j];
		}

		/* Addition */
		Matrix3<T> operator+(T value) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] + value;
			return mat;
		}

		Matrix3<T> operator+=(T value)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] += value;
			return *this;
		}

		Matrix3<T> operator+(const Matrix3<T>& matrix3) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] + matrix3.matrix[i][j];
			return mat;
		}

		Matrix3<T> operator+=(const Matrix3<T>& matrix3)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] += matrix3.matrix[i][j];
			return *this;
		}

		Matrix3<T> operator+(const Vec3& vector) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
			{
				mat.matrix[i][0] = matrix[i][0] + vector.x;
				mat.matrix[i][1] = matrix[i][1] + vector.y;
				mat.matrix[i][2] = matrix[i][2] + vector.z;
			}
			return mat;
		}

		Matrix3<T> operator+=(const Vec3& vector)
		{
			for (size_t i = 0; i < 3; i++)
			{
				matrix[i][0] += vector.x;
				matrix[i][1] += vector.y;
				matrix[i][2] += vector.z;
			}
			return *this;
		}

		/* Subtraction */
		Matrix3<T> operator-(T value) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] - value;
			return mat;
		}

		Matrix3<T> operator-=(T value)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] -= value;
			return *this;
		}

		Matrix3<T> operator-(const Matrix3<T>& matrix3) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] - matrix3.matrix[i][j];
			return mat;
		}

		Matrix3<T> operator-=(const Matrix3<T>& matrix3)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] -= matrix3.matrix[i][j];
			return *this;
		}

		Matrix3<T> operator-(const Vec3& vector) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
			{
				mat.matrix[i][0] = matrix[i][0] - vector.x;
				mat.matrix[i][1] = matrix[i][1] - vector.y;
				mat.matrix[i][2] = matrix[i][2] - vector.z;
			}
			return mat;
		}

		Matrix3<T> operator-=(const Vec3& vector)
		{
			for (size_t i = 0; i < 3; i++)
			{
				matrix[i][0] -= vector.x;
				matrix[i][1] -= vector.y;
				matrix[i][2] -= vector.z;
			}
			return *this;
		}

		/* Multiplication */
		Matrix3<T> operator*(T value) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] * value;
			return mat;
		}

		Matrix3<T> operator*=(T value)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] *= value;
			return *this;
		}

		Matrix3<T> operator*(const Matrix3<T>& matrix3) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] * matrix3.matrix[j][i];
			return mat;
		}

		Matrix3<T> operator*=(const Matrix3<T>& matrix3)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] *= matrix3.matrix[j][i];
			return *this;
		}

		Matrix3<T> operator*(const Vec3& vector) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
			{
				mat.matrix[i][0] = matrix[i][0] * vector.x;
				mat.matrix[i][1] = matrix[i][1] * vector.y;
				mat.matrix[i][2] = matrix[i][2] * vector.z;
			}
			return mat;
		}

		Vector3<T> operator*(Vec3& vector) const
		{
			Vector3<T> vec = Vector3<T>();

			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					vec[i] += matrix[i][j] * vector[j];

			return vec;
		}

		Matrix3<T> operator*=(const Vec3& vector)
		{
			for (size_t i = 0; i < 3; i++)
			{
				matrix[i][0] *= vector.x;
				matrix[i][1] *= vector.y;
				matrix[i][2] *= vector.z;
			}
			return *this;
		}

		/* Division */
		Matrix3<T> operator/(T value) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] / value;
			return mat;
		}

		Matrix3<T> operator/=(T value)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] /= value;
			return *this;
		}

		Matrix3<T> operator/(const Matrix3<T>& matrix3) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					mat.matrix[i][j] = matrix[i][j] / matrix3.matrix[i][j];
			return mat;
		}

		Matrix3<T> operator/=(const Matrix3<T>& matrix3)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					matrix[i][j] /= matrix3.matrix[i][j];
			return *this;
		}

		Matrix3<T> operator/(const Vec3& vector) const
		{
			Matrix3<T> mat;
			for (size_t i = 0; i < 3; i++)
			{
				mat.matrix[i][0] = matrix[i][0] / vector.x;
				mat.matrix[i][1] = matrix[i][1] / vector.y;
				mat.matrix[i][2] = matrix[i][2] / vector.z;
			}
			return mat;
		}

		Matrix3<T> operator/=(const Vec3& vector)
		{
			for (size_t i = 0; i < 3; i++)
			{
				matrix[i][0] /= vector.x;
				matrix[i][1] /= vector.y;
				matrix[i][2] /= vector.z;
			}
			return *this;
		}

		/* Comparsion */
		bool operator==(const Matrix3<T>& matrix3)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					if (matrix[i][j] != matrix3.matrix[i][j])
						return false;
			return true;
		}

		bool operator!=(const Matrix3<T>& matrix3)
		{
			for (size_t i = 0; i < 3; i++)
				for (size_t j = 0; j < 3; j++)
					if (matrix[i][j] != matrix3.matrix[i][j])
						return true;
			return false;
		}

		float* operator[](size_t i) { return matrix[i]; }

		template <typename U>
		friend std::ostream& operator<<(std::ostream& os, const Matrix3<U>& matrix);
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Matrix3<T>& matrix)
	{
		for (size_t i = 0; i < 3; i++)
		{
			os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << ", " << matrix.matrix[i][2] << "]";
			if (i != 3)
				os << std::endl;
		}
		return os;
	}
}
