////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Variable.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <string>
#include <unordered_set>

namespace tiler
{
    // Represents a variable (loop index, matrix, etc) in a loop nest
    class Variable
    {
    public:
        // Constructs a variable
        Variable();

        // Determines equality to another variable 
        bool operator==(const Variable& other) const;

        // Returns the variable name
        std::string GetName() const { return "v" + std::to_string(_id); }

    protected:
        static int _counter;
        int _id;
    };
}
