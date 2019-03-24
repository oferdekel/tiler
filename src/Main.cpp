////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Main.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IndentedOutputStream.h"
#include "Kernel.h"
#include "Nest.h"
#include "Matrix.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace tiler;

int main(int argc, char** argv)
{
    try
    {
        std::vector<float> u = MatrixToVector(MatrixOrder::rowMajor,
        { {1, 5, 3, 4, 7, 1},
          {7, 0, 7, 8, 5, 1},
          {1, 3, 2, 7, 0, 1},
          {5, 4, 7, 8, 5, 4} });

        std::vector<float> v = MatrixToVector(MatrixOrder::rowMajor,
        { {6, 3, 3, 9},
          {7, 3, 7, 8},
          {1, 0, 6, 1},
          {2, 8, 0, 1},
          {1, 4, 0, 1},
          {7, 1, 5, 7} });

        std::vector<float> z(16);

        Variable i, j, k;
        Variable A, B, C, AA, BB, CC;

        Using(A, {4, 6, MatrixOrder::rowMajor}, u.data())
        .Using(B, {6, 4, MatrixOrder::rowMajor}, v.data())
        .Using(C, {4, 4, MatrixOrder::rowMajor}, z.data())
        .ForAll(i, 0, 4, 2)
            .ForAll(j, 0, 4, 2)
                .ForAll(k, 0, 6, 2)
                    .Tile(AA, A, i, k, 2, 2)
                    .Tile(BB, B, k, j, 2, 2)
                    .Tile(CC, C, i, j, 2, 2)
                    .Kernel(AA, BB, CC, MMKernelRRR222)
                    .Print(std::cout);
    }
    catch(std::logic_error e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}