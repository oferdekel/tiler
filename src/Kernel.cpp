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
    void MMKernel222(std::ostream& stream, const MatrixStatement& matrixA, const MatrixStatement& matrixB, const MatrixStatement& matrixC)
    {
        // check matrix A compatibility with kernel
        auto a = matrixA.GetLayout();
        if(a.NumRows() != 2 || a.NumColumns() != 2)
        {
            throw std::logic_error("matrix A incompatible with kernel requirements");
        }

        // check matrix B compatibility with kernel
        auto b = matrixB.GetLayout();
        if(b.NumRows() != 2 || b.NumColumns() != 2)
        {
            throw std::logic_error("matrix B incompatible with kernel requirements");
        }

        // check matrix C compatibility with kernel
        auto c = matrixC.GetLayout();
        if(c.NumRows() != 2 || c.NumColumns() != 2)
        {
            throw std::logic_error("matrix C incompatible with kernel requirements");
        }

        auto A = matrixA.GetVariable().GetName();
        auto B = matrixB.GetVariable().GetName();
        auto C = matrixC.GetVariable().GetName();

        stream << endil << "// matrix-matrix multiplication kernel with three 2x2 matrices" << endil;
        stream << "(*(" << C << "+" << c(0,0) << ")) += (*(" << A << "+" << a(0,0) << ")) * (*(" << B << "+" << b(0,0) << ")) + (*(" << A << "+" << a(0,1) << ")) * (*(" << B << "+" << b(1,0) << "));" << endil;
        stream << "(*(" << C << "+" << c(0,1) << ")) += (*(" << A << "+" << a(0,0) << ")) * (*(" << B << "+" << b(0,1) << ")) + (*(" << A << "+" << a(0,1) << ")) * (*(" << B << "+" << b(1,1) << "));" << endil;
        stream << "(*(" << C << "+" << c(1,0) << ")) += (*(" << A << "+" << a(1,0) << ")) * (*(" << B << "+" << b(0,0) << ")) + (*(" << A << "+" << a(1,1) << ")) * (*(" << B << "+" << b(1,0) << "));" << endil;
        stream << "(*(" << C << "+" << c(1,1) << ")) += (*(" << A << "+" << a(1,0) << ")) * (*(" << B << "+" << b(0,1) << ")) + (*(" << A << "+" << a(1,1) << ")) * (*(" << B << "+" << b(1,1) << "));";
    } 
}