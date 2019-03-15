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

    std::cout << 
    ForAll(i, 0, 10, 1)
        .ForAll(j, 0, 10, 1)        .Order(12)
            .ForAll(k, 0, 10, 1)
                .NestSize();

    return 0;
}