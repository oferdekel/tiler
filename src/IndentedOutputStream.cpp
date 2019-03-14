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
    IndentedOutputStream::IndentedOutputStream(std::ostream& stream) : _stream(stream)
    {}

    IndentedOutputStream& IndentedOutputStream::operator<<(Endl) 
    {
        _stream.get() << std::endl;
        _lineBeginning = true;
        return *this;
    }
}