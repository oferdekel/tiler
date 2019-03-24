////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Kernel.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Kernel.h"

namespace tiler
{
    void MMKernelRRR222(std::ostream& stream, const MatrixLayout& matrixA, const MatrixLayout& matrixB, const MatrixLayout& matrixC)
    {
        stream << "test "
            << matrixA.GetName()
            << " "
            << matrixB.GetName()
            << " "
            << matrixC.GetName();
    } 
}