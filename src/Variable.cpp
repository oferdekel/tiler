////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Variable.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variable.h"

namespace tiler
{
    // initialize the variable counter
    int Variable::_counter = 0;

    Variable::Variable() : _id(_counter++)
    {}

    bool Variable::operator==(const Variable& other) const
    {
        return _id == other._id;
    }

    std::ostream& operator<<(std::ostream& stream, const Variable& variable)
    {
        stream << 'v' << variable.GetID();
        return stream;
    }
}