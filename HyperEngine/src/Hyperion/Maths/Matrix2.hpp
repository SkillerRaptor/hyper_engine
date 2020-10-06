#pragma once

#include <cmath>
#include <iostream>

#include "Vector2.hpp"

namespace Hyperion
{
	template <typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	class Matrix2
	{
	public:
		T matrix[2][2];

	public:
		Matrix2()
			: Matrix2(0.0f)
		{
		}

		Matrix2(T value)
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

		Matrix2<T> operator+(const Vector2<float>& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] + vector.x;
				mat.matrix[i][1] = matrix[i][1] + vector.y;
			}
			return mat;
		}

		Matrix2<T> operator+=(const Vector2<float>& vector)
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

		Matrix2<T> operator-(const Vector2<float>& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] - vector.x;
				mat.matrix[i][1] = matrix[i][1] - vector.y;
			}
			return mat;
		}

		Matrix2<T> operator-=(const Vector2<float>& vector)
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
					mat.matrix[i][j] = matrix[i][j] * matrix2.matrix[i][j];
			return mat;
		}

		Matrix2<T> operator*=(const Matrix2<T>& matrix2)
		{
			for (size_t i = 0; i < 2; i++)
				for (size_t j = 0; j < 2; j++)
					matrix[i][j] *= matrix2.matrix[i][j];
			return *this;
		}

		Matrix2<T> operator*(const Vector2<float>& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] * vector.x;
				mat.matrix[i][1] = matrix[i][1] * vector.y;
			}
			return mat;
		}

		Matrix2<T> operator*=(const Vector2<float>& vector)
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

		Matrix2<T> operator/(const Vector2<float>& vector) const
		{
			Matrix2<T> mat;
			for (size_t i = 0; i < 2; i++)
			{
				mat.matrix[i][0] = matrix[i][0] / vector.x;
				mat.matrix[i][1] = matrix[i][1] / vector.y;
			}
			return mat;
		}

		Matrix2<T> operator/=(const Vector2<float>& vector)
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

		friend std::ostream& operator<<(std::ostream& os, const Matrix2<T>& matrix);
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Matrix2<T>& matrix)
	{
		for (int i = 0; i < 2; i++)
		{
			os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << "]";
			if (i != 3)
				os << std::endl;
		}
		return os;
	}
}