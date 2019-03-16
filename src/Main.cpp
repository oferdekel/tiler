////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Main.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
        auto u = MatrixToVector(MatrixOrder::rowMajor,
        { {1, 2, 3},
          {4, 5, 6} }
        );

        Matrix A(u.data(), 2, 3, MatrixOrder::rowMajor);

        Variable i, j, k, l;
        Variable B;

        Using(A)
        .ForAll(i, 0, 10, 1)
            .ForAll(j, 0, 20, 2)        .Position(-3)
            .Tile(B, A, i, j, 2, 2)
                .ForAll(k, 0, 30, 3)
                    .Print(std::cout);
    }
    catch(std::logic_error e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}