#pragma once

#include <cmath>
#include <iostream>

#include "HyperMath/Vector.hpp"

namespace Hyperion
{
	template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	class Matrix2
	{
	public:
		T matrix[2][2];

	public:
		Matrix2()
			: Matrix2(0.0f)
		{
		}

		explicit Matrix2(T value)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
				{
					matrix[i][j] = 0.0f;
					if (i == j)
						matrix[i][j] = value;
				}
		}

		Matrix2(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] = matrix2.matrix[i][j];
		}

		/* Addition */
		Matrix2<T> operator+(T value) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] + value;
			return mat;
		}

		Matrix2<T> operator+=(T value)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] += value;
			return *this;
		}

		Matrix2<T> operator+(const Matrix2<T>& matrix2) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] + matrix2.matrix[i][j];
			return mat;
		}

		Matrix2<T> operator+=(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] += matrix2.matrix[i][j];
			return *this;
		}

		Matrix2<T> operator+(const Vec2& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] + vector.x;
				mat.matrix[i][1] = matrix[i][1] + vector.y;
			}
			return mat;
		}

		Matrix2<T> operator+=(const Vec2& vector)
		{
			for (size_t i = 0; i < 2; i++)
			{
				matrix[i][0] += vector.x;
				matrix[i][1] += vector.y;
			}
			return *this;
		}

		/* Subtraction */
		Matrix2<T> operator-(T value) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] - value;
			return mat;
		}

		Matrix2<T> operator-=(T value)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] -= value;
			return *this;
		}

		Matrix2<T> operator-(const Matrix2<T>& matrix2) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] - matrix2.matrix[i][j];
			return mat;
		}

		Matrix2<T> operator-=(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] -= matrix2.matrix[i][j];
			return *this;
		}

		Matrix2<T> operator-(const Vec2& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] - vector.x;
				mat.matrix[i][1] = matrix[i][1] - vector.y;
			}
			return mat;
		}

		Matrix2<T> operator-=(const Vec2& vector)
		{
			for (size_t i = 0; i < 2; i++)
			{
				matrix[i][0] -= vector.x;
				matrix[i][1] -= vector.y;
			}
			return *this;
		}

		/* Multiplication */
		Matrix2<T> operator*(T value) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] * value;
			return mat;
		}

		Matrix2<T> operator*=(T value)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] *= value;
			return *this;
		}

		Matrix2<T> operator*(const Matrix2<T>& matrix2) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] * matrix2.matrix[j][i];
			return mat;
		}

		Matrix2<T> operator*=(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] *= matrix2.matrix[j][i];
			return *this;
		}

		Matrix2<T> operator*(const Vec2& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] * vector.x;
				mat.matrix[i][1] = matrix[i][1] * vector.y;
			}
			return mat;
		}

		Vector2<T> operator*(Vec2& vector) const
		{
			Vector2<T> vec = Vector2<T>();

			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					vec[i] += matrix[i][j] * vector[j];

			return vec;
		}

		Matrix2<T> operator*=(const Vec2& vector)
		{
			for (size_t i = 0; i < 2; i++)
			{
				matrix[i][0] *= vector.x;
				matrix[i][1] *= vector.y;
			}
			return *this;
		}

		/* Division */
		Matrix2<T> operator/(T value) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] / value;
			return mat;
		}

		Matrix2<T> operator/=(T value)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] /= value;
			return *this;
		}

		Matrix2<T> operator/(const Matrix2<T>& matrix2) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					mat.matrix[i][j] = matrix[i][j] / matrix2.matrix[i][j];
			return mat;
		}

		Matrix2<T> operator/=(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] /= matrix2.matrix[i][j];
			return *this;
		}

		Matrix2<T> operator/(const Vec2& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] / vector.x;
				mat.matrix[i][1] = matrix[i][1] / vector.y;
			}
			return mat;
		}

		Matrix2<T> operator/=(const Vec2& vector)
		{
			for (size_t i = 0; i < 2; i++)
			{
				matrix[i][0] /= vector.x;
				matrix[i][1] /= vector.y;
			}
			return *this;
		}

		/* Comparsion */
		bool operator==(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					if (matrix[i][j] != matrix2.matrix[i][j])
						return false;
			return true;
		}

		bool operator!=(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					if (matrix[i][j] != matrix2.matrix[i][j])
						return true;
			return false;
		}

		float* operator[](size_t i) { return matrix[i]; }
	};
}
