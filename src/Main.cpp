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
        Variable A, B, C, D;
        Variable i, j, k, l;

        Using(A).Using(B).Using(C)
        .ForAll(i, 0, 10, 1)
            .ForAll(j, 0, 20, 2)        .Position(-3)
            .Tile(D, A, i, j, 2, 2)
                .ForAll(k, 0, 30, 3)
                    .Print(std::cout);
    }
    catch(std::logic_error e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}