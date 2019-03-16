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
#include <string>
#include <vector>

using namespace tiler;

int main(int argc, char** argv)
{
    Variable A, B, C;
    Variable i, j, k;

    Using(A).Using(B).Using(C)
    .ForAll(i, 0, 10, 1)
        .ForAll(j, 0, 20, 2)        .Position(-3)
            .ForAll(k, 0, 30, 3)
                .Print(std::cout);

    return 0;
}