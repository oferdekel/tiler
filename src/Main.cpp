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
    Variable i, j, k;

    std::cout << i << " " << j << " " << k << std::endl;

    ForAll(i, 0, 10, 1)
        .ForAll(j, 0, 10, 1)        .Position(12)
            .ForAll(k, 0, 10, 1)
                .Print(std::cout);

    return 0;
}