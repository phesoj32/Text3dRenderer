#include "3dRenderer.h"
#include <array>
namespace MatrixCalc {


    using Mat3x3 = std::array<float, 9>;
    using Mat3x1 = std::array<float, 3>;

    Mat3x3 multiply3x3(const Mat3x3& A, const Mat3x3& B)
    {
        return {
            A[0] * B[0] + A[1] * B[3] + A[2] * B[6],
            A[0] * B[1] + A[1] * B[4] + A[2] * B[7],
            A[0] * B[2] + A[1] * B[5] + A[2] * B[8],

            A[3] * B[0] + A[4] * B[3] + A[5] * B[6],
            A[3] * B[1] + A[4] * B[4] + A[5] * B[7],
            A[3] * B[2] + A[4] * B[5] + A[5] * B[8],

            A[6] * B[0] + A[7] * B[3] + A[8] * B[6],
            A[6] * B[1] + A[7] * B[4] + A[8] * B[7],
            A[6] * B[2] + A[7] * B[5] + A[8] * B[8]
        };
    }
    void multiplyMatrices(
        const float* A,
        const float* B,
        float* C,
        int rowsA,
        int colsA,
        int colsB)
    {
        std::fill(C, C + rowsA * colsB, 0.0f);

        for (int i = 0; i < rowsA; ++i)
        {
            const float* aRow = A + i * colsA;
            float* cRow = C + i * colsB;

            for (int k = 0; k < colsA; ++k)
            {
                const float a = aRow[k];
                const float* bRow = B + k * colsB;

                for (int j = 0; j < colsB; ++j)
                {
                    cRow[j] += a * bRow[j];
                }
            }
        }
    }
    Mat3x1 matMult3x3_3x1(const Mat3x1& A, const mat3x3& B)
    {
        return {
            B[0] * A[0] + B[1] * A[1] + B[2] * A[2],
            B[3] * A[0] + B[4] * A[1] + B[5] * A[2],
            B[6] * A[0] + B[7] * A[1] + B[8] * A[2]
        };
    }
}