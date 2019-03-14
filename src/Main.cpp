////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Main.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IndentedOutputStream.h"

#include <iostream>
#include <string>
#include <vector>

using namespace tiler;

int main(int argc, char** argv)
{

    IndentedOutputStream printer(std::cout);
    printer << 3 << "x" << endl;
    printer.IncreaseIndent();
    printer << "ddddd" << 3 << "x" << endl;
    printer << 300 << "x" << endl;
    printer.IncreaseIndent();
    printer << "ddddd" << 3 << "x" << endl;
    printer << 300 << "x" << endl;
    printer.DecreaseIndent();
    printer << "ddddd" << 3 << "x" << endl;
    printer << 300 << "x" << endl;
    

    return 0;
}