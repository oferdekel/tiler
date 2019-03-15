////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Main.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForAll.h"
#include "Matrix.h"

#include <iostream>
#include <string>
#include <vector>

using namespace tiler;

int main(int argc, char** argv)
{

    Index i, j, k;

    ForAll(i, 0, 10, 1)
        .ForAll(j, 0, 10, 1).Order(-1)
            .ForAll(k, 0, 10, 1)
                .Print(std::cout);


    return 0;
}