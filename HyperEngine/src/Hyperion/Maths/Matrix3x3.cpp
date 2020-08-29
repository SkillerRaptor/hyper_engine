#include "Matrix3x3.h"

namespace Hyperion {

    Matrix3x3::Matrix3x3()
    {
        Matrix3x3(0.0f);
    }

	Matrix3x3::Matrix3x3(float value)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                matrix[i][j] = 0;
                if (i == j)
                    matrix[i][j] = value;
            }
        }
    }

    Matrix3x3::Matrix3x3(const Matrix3x3& mat)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                matrix[i][j] = mat.matrix[i][j];
            }
        }
    }

    std::ostream& operator<<(std::ostream& os, const Matrix3x3& matrix)
    {
        for (int i = 0; i < 3; i++)
        {
            os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << ", " << matrix.matrix[i][2] << "]";
            if (i != 2)
                os << std::endl;
        }
        return os;
    }
}