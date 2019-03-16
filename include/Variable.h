////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Variable.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>

namespace tiler
{
    // An index for a forall loop
    class Variable
    {
    public:
        // constructs a variable
        Variable();

        // equality to another variable 
        bool operator==(const Variable& other);

        // returns the ID of the variable
        int GetID() const { return _id; }

    protected:
        static int _counter;
        int _id;
    };

    // Outputs the name of a variable to a stream
    std::ostream& operator<<(std::ostream& stream, const Variable& variable);
}
