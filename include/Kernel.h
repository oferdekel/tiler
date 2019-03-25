////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Kernel.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Statement.h"

#include <iostream>

namespace tiler
{
    // 2x2x2 matrix multiplication kernel 
    void MMKernel222(std::ostream& stream, const MatrixStatement& matrixA, const MatrixStatement& matrixB, const MatrixStatement& matrixC);
}