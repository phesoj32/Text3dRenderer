#pragma once
#include <vector>
#include <array>

namespace MatrixCalc
{
	using mat3x3 = std::array<float, 9>;
    using Mat3x1 = std::array<float, 3>;
	mat3x3 multiply3x3(const mat3x3& A, const mat3x3& B);
    void multiplyMatrices(
        const float* A,
        const float* B,
        float* C,
        int rowsA,
        int colsA,
        int colsB);
    Mat3x1 matMult3x3_3x1(const Mat3x1& A, const mat3x3& B);
}