#pragma once

#include <iostream>
#include <cmath>

#include "Vector3.h"

namespace Hyperion {

    class Matrix3
    {
    public:
        float matrix[3][3];

    public:
        Matrix3();

        Matrix3(float value);

        Matrix3(const Matrix3& mat);

        Matrix3 operator+(Matrix3 matrixValue) const
        {
            Matrix3 mat;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    mat.matrix[i][j] = matrix[i][j] + matrixValue.matrix[i][j];
                }
            }
            return mat;
        }

        Matrix3 operator-(Matrix3 matrixValue) const
        {
            Matrix3 mat;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    mat.matrix[i][j] = matrix[i][j] - matrixValue.matrix[i][j];
                }
            }
            return mat;
        }

        Matrix3 operator*(float value) const
        {
            Matrix3 mat;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    mat.matrix[i][j] = value * matrix[i][j];
                }
            }
            return mat;
        }

        Matrix3 operator*(Matrix3 matrixValue) const
        {
            Matrix3 mat;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    mat.matrix[i][j] = matrix[i][j] * matrixValue.matrix[j][i];
                }
            }
            return mat;
        }

        Matrix3 operator*(Vector3& vector) const
        {
            Matrix3 mat;
            for (int i = 0; i < 3; i++)
            {
                mat.matrix[i][0] = matrix[i][0] * vector.x;
                mat.matrix[i][1] = matrix[i][1] * vector.y;
                mat.matrix[i][2] = matrix[i][2] * vector.z;
            }
            return mat;
        }

        friend std::ostream& operator<<(std::ostream& os, const Matrix3& matrix);
    };
}