////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Kernel.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IndentedOutputStream.h"
#include "Kernel.h"
#include "Matrix.h"

#include <stdexcept>

namespace tiler
{
    void MMKernelRRR222(std::ostream& stream, const MatrixStatement& matrixA, const MatrixStatement& matrixB, const MatrixStatement& matrixC)
    {
        // check matrix A compatibility with kernel
        auto matrixALayout = matrixA.GetLayout();
        if(matrixALayout.GetOrder() != MatrixOrder::rowMajor || matrixALayout.NumRows() != 2 || matrixALayout.NumColumns() != 2)
        {
            throw std::logic_error("matrix A incompatible with kernel requirements");
        }

        // check matrix B compatibility with kernel
        auto matrixBLayout = matrixB.GetLayout();
        if(matrixBLayout.GetOrder() != MatrixOrder::rowMajor || matrixBLayout.NumRows() != 2 || matrixBLayout.NumColumns() != 2)
        {
            throw std::logic_error("matrix B incompatible with kernel requirements");
        }

        // check matrix C compatibility with kernel
        auto matrixCLayout = matrixC.GetLayout();
        if(matrixCLayout.GetOrder() != MatrixOrder::rowMajor || matrixCLayout.NumRows() != 2 || matrixCLayout.NumColumns() != 2)
        {
            throw std::logic_error("matrix C incompatible with kernel requirements");
        }

        auto A = matrixA.GetVariable().GetName();
        auto B = matrixB.GetVariable().GetName();
        auto C = matrixC.GetVariable().GetName();
        auto lda = matrixALayout.GetLeadingDimensionSize();
        auto ldb = matrixBLayout.GetLeadingDimensionSize();
        auto ldc = matrixCLayout.GetLeadingDimensionSize();

        stream << endil << "// matrix-matrix multiplication kernel with three 2x2 row-major matrices" << endil;
        stream << "(*" << C << ") += (*" << A << ") * (*" << B << ") + (*(" << A << "+1)) * (*(" << B << "+" << ldb << "));" << endil;
        stream << "(*(" << C << "+1)) += (*" << A << ") * (*(" << B << "+1)) + (*(" << A << "+1)) * (*(" << B << "+" << ldb+1 << "));" << endil;
        stream << "(*(" << C << "+" << ldc << ")) += (*(" << A << "+" << lda << ")) * (*" << B << ") + (*(" << A << "+" << lda+1 << ")) * (*(" << B << "+" << ldb << "));" << endil;
        stream << "(*(" << C << "+" << ldc+1 << ")) += (*(" << A << "+" << lda << ")) * (*(" << B << "+1)) + (*(" << A << "+" << lda+1 << ")) * (*(" << B << "+" << ldb+1 << "));";
    } 
}