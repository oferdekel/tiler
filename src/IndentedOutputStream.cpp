////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     IndentedOutputStream.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IndentedOutputStream.h"


namespace tiler
{
    int globalIndentLevel = 0;


    std::ostream& endil(std::ostream& stream)
    {
        stream << std::endl;
        stream << std::string(4 * globalIndentLevel, ' ');
        return stream;
    }
 
    void IncreaseIndent() { globalIndentLevel++; }
    void DecreaseIndent() { globalIndentLevel--; }
}
