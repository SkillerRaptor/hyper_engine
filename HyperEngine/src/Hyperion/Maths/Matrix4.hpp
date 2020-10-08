#pragma once

#include <cmath>
#include <iostream>

#include "Vector4.hpp"

namespace Hyperion 
{
	template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    class Matrix4
    {
    public:
        T matrix[4][4];

    public:
		Matrix4()
			: Matrix4(0.0f)
		{
		}

		Matrix4(T value)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
				{
					matrix[i][j] = 0.0f;
					if (i == j)
						matrix[i][j] = value;
				}
		}

		Matrix4(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] = matrix4.matrix[i][j];
		}

		/* Addition */
		Matrix4<T> operator+(T value) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] + value;
			return mat;
		}

		Matrix4<T> operator+=(T value)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] += value;
			return *this;
		}

		Matrix4<T> operator+(const Matrix4<T>& matrix4) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] + matrix4.matrix[i][j];
			return mat;
		}

		Matrix4<T> operator+=(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] += matrix4.matrix[i][j];
			return *this;
		}

		Matrix4<T> operator+(const Vector4<float>& vector) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
			{
				mat.matrix[i][0] = matrix[i][0] + vector.x;
				mat.matrix[i][1] = matrix[i][1] + vector.y;
				mat.matrix[i][2] = matrix[i][2] + vector.z;
				mat.matrix[i][3] = matrix[i][3] + vector.w;
			}
			return mat;
		}

		Matrix4<T> operator+=(const Vector4<float>& vector)
		{
			for (size_t i = 0; i < 4; i++)
			{
				matrix[i][0] += vector.x;
				matrix[i][1] += vector.y;
				matrix[i][2] += vector.z;
				matrix[i][3] += vector.w;
			}
			return *this;
		}

		/* Subtraction */
		Matrix4<T> operator-(T value) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] - value;
			return mat;
		}

		Matrix4<T> operator-=(T value)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] -= value;
			return *this;
		}

		Matrix4<T> operator-(const Matrix4<T>& matrix4) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] - matrix4.matrix[i][j];
			return mat;
		}

		Matrix4<T> operator-=(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] -= matrix4.matrix[i][j];
			return *this;
		}

		Matrix4<T> operator-(const Vector4<float>& vector) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
			{
				mat.matrix[i][0] = matrix[i][0] - vector.x;
				mat.matrix[i][1] = matrix[i][1] - vector.y;
				mat.matrix[i][2] = matrix[i][2] - vector.z;
				mat.matrix[i][3] = matrix[i][3] - vector.w;
			}
			return mat;
		}

		Matrix4<T> operator-=(const Vector4<float>& vector)
		{
			for (size_t i = 0; i < 4; i++)
			{
				matrix[i][0] -= vector.x;
				matrix[i][1] -= vector.y;
				matrix[i][2] -= vector.z;
				matrix[i][3] -= vector.w;
			}
			return *this;
		}

		/* Multiplication */
		Matrix4<T> operator*(T value) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] * value;
			return mat;
		}

		Matrix4<T> operator*=(T value)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] *= value;
			return *this;
		}

		Matrix4<T> operator*(const Matrix4<T>& matrix4) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] * matrix4.matrix[i][j];
			return mat;
		}

		Matrix4<T> operator*=(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] *= matrix4.matrix[i][j];
			return *this;
		}
		
		Vector4<T> operator*(const Vector4<float>& vector) const
		{
			Vector4<T> vec = Vector4<T>();

			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					vec[i] += matrix[i][j] * vector[i];

			return vec;
		}

		Matrix4<T> operator*=(const Vector4<float>& vector)
		{
			for (size_t i = 0; i < 4; i++)
			{
				matrix[i][0] *= vector.x;
				matrix[i][1] *= vector.y;
				matrix[i][2] *= vector.z;
				matrix[i][3] *= vector.w;
			}
			return *this;
		}

		/* Division */
		Matrix4<T> operator/(T value) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] / value;
			return mat;
		}

		Matrix4<T> operator/=(T value)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] /= value;
			return *this;
		}

		Matrix4<T> operator/(const Matrix4<T>& matrix4) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.matrix[i][j] = matrix[i][j] / matrix4.matrix[i][j];
			return mat;
		}

		Matrix4<T> operator/=(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] /= matrix4.matrix[i][j];
			return *this;
		}

		Matrix4<T> operator/(const Vector4<float>& vector) const
		{
			Matrix4<T> mat;
			for (size_t i = 0; i < 4; i++)
			{
				mat.matrix[i][0] = matrix[i][0] / vector.x;
				mat.matrix[i][1] = matrix[i][1] / vector.y;
				mat.matrix[i][2] = matrix[i][2] / vector.z;
				mat.matrix[i][3] = matrix[i][3] / vector.w;
			}
			return mat;
		}

		Matrix4<T> operator/=(const Vector4<float>& vector)
		{
			for (size_t i = 0; i < 4; i++)
			{
				matrix[i][0] /= vector.x;
				matrix[i][1] /= vector.y;
				matrix[i][2] /= vector.z;
				matrix[i][3] /= vector.w;
			}
			return *this;
		}

		/* Comparsion */
		bool operator==(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					if (matrix[i][j] != matrix4.matrix[i][j])
						return false;
			return true;
		}

		bool operator!=(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					if (matrix[i][j] != matrix4.matrix[i][j])
						return true;
			return false;
		}

		float* operator[](size_t i) { return matrix[i]; }

		template <typename U>
        friend std::ostream& operator<<(std::ostream& os, const Matrix4<U>& matrix);
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Matrix4<T>& matrix)
	{
		for (size_t i = 0; i < 4; i++)
		{
			os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << ", " << matrix.matrix[i][2] << ", " << matrix.matrix[i][3] << "]";
			if (i != 3)
				os << std::endl;
		}
		return os;
	}
}
