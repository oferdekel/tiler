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
    void MMKernelRRR222(std::ostream& stream, const MatrixStatement& matrixA, const MatrixStatement& matrixB, const MatrixStatement& matrixC)
    {
        stream << "test "
            << matrixA.GetVariable().GetName()
            << " "
            << matrixB.GetVariable().GetName()
            << " "
            << matrixC.GetVariable().GetName();
    } 
}