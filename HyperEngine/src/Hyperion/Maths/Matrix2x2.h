#pragma once

#include <iostream>
#include <cmath>

#include "Vector2.h"

namespace Hyperion {
    class Matrix2
    {
    public:
        float matrix[2][2];

    public:
        Matrix2();

        Matrix2(float value);

        Matrix2(const Matrix2& mat);

        Matrix2 operator+(Matrix2 matrixValue) const
        {
            Matrix2 mat;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    mat.matrix[i][j] = matrix[i][j] + matrixValue.matrix[i][j];
                }
            }
            return mat;
        }

        Matrix2 operator-(Matrix2 matrixValue) const
        {
            Matrix2 mat;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    mat.matrix[i][j] = matrix[i][j] - matrixValue.matrix[i][j];
                }
            }
            return mat;
        }

        Matrix2 operator*(float value) const
        {
            Matrix2 mat;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    mat.matrix[i][j] = value * matrix[i][j];
                }
            }
            return mat;
        }

        Matrix2 operator*(Matrix2 matrixValue) const
        {
            Matrix2 mat;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    mat.matrix[i][j] = matrix[i][j] * matrixValue.matrix[j][i];
                }
            }
            return mat;
        }

        Matrix2 operator*(Vector2& vector) const
        {
            Matrix2 mat;
            for (int i = 0; i < 2; i++)
            {
                mat.matrix[i][0] = matrix[i][0] * vector.x;
                mat.matrix[i][1] = matrix[i][1] * vector.y;
            }
            return mat;
        }

        friend std::ostream& operator<<(std::ostream& os, const Matrix2& matrix);
    };
}