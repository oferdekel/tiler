////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     PrintUtils.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintUtils.h"


namespace tiler
{
    int globalIndentLevel = 0;

    std::ostream& endil(std::ostream& stream)
    {
        stream << std::endl;
        stream << std::string(4 * globalIndentLevel, ' ');
        return stream;
    }
 
    void IncreaseIndent() 
    { 
        globalIndentLevel++; 
    }  
    
    void DecreaseIndent() 
    { 
        globalIndentLevel--; 
    }

    void PrintFormated(std::ostream& os, const char* format)
    {
        while (*format != '\0')
        {
            os << *format;
            ++format;
        }
    }
}
