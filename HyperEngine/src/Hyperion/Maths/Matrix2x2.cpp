#include "Matrix2x2.h"

namespace Hyperion {

    Matrix2x2::Matrix2x2()
    {
        Matrix2x2(0.0f);
    }

    Matrix2x2::Matrix2x2(float value)
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                matrix[i][j] = 0;
                if (i == j)
                    matrix[i][j] = value;
            }
        }
    }

    Matrix2x2::Matrix2x2(const Matrix2x2& mat)
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                matrix[i][j] = mat.matrix[i][j];
            }
        }
    }

    std::ostream& operator<<(std::ostream& os, const Matrix2x2& matrix)
    {
        for (int i = 0; i < 2; i++)
        {
            os << "[" << matrix.matrix[i][0] << ", " << matrix.matrix[i][1] << "]";
            if (i != 1)
                os << std::endl;
        }
        return os;
    }
}