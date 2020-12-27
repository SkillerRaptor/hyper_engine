#pragma once

#include <cmath>
#include <iostream>

#include "Vector.hpp"

namespace Hyperion 
{
	template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    class Matrix4
    {
    public:
        T matrix[4][4];

    public:
		Matrix4()
			: Matrix4(0.0f)
		{
		}

		explicit Matrix4(T value)
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

		Matrix4<T> operator+(const Vec4& vector) const
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

		Matrix4<T> operator+=(const Vec4& vector)
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

		Matrix4<T> operator-(const Vec4& vector) const
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

		Matrix4<T> operator-=(const Vec4& vector)
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
					mat.matrix[i][j] = matrix[i][j] * matrix4.matrix[j][i];
			return mat;
		}

		Matrix4<T> operator*=(const Matrix4<T>& matrix4)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					matrix[i][j] *= matrix4.matrix[j][i];
			return *this;
		}
		
		Vector4<T> operator*(Vec4& vector) const
		{
			Vector4<T> vec = Vector4<T>();
		
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					vec[i] += matrix[i][j] * vector[j];
		
			return vec;
		}

		Matrix4<T> operator*=(const Vec4& vector)
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

		Matrix4<T> operator/(const Vec4& vector) const
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

		Matrix4<T> operator/=(const Vec4& vector)
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

		/* Conversion */
		float* operator[](size_t i) 
		{ 
			return matrix[i]; 
		}
	};
}
