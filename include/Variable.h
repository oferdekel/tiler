////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Variable.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <unordered_set>

namespace tiler
{
    // An index for a forall loop
    class Variable
    {
    public:
        // constructs a variable
        Variable();

        // equality to another variable 
        bool operator==(const Variable& other) const;

        // returns the ID of the variable
        int GetID() const { return _id; }

    protected:
        static int _counter;
        int _id;
    };

    // Outputs the name of a variable to a stream
    std::ostream& operator<<(std::ostream& stream, const Variable& variable);
}

namespace std {
    template<>
    struct hash<tiler::Variable>
    {
        std::size_t operator()(const tiler::Variable& variable) const noexcept
        {
            return variable.GetID();
        }
    };
}
