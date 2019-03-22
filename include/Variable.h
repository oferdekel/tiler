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

        // returns the variable name
        std::string GetName() const { return "v" + std::to_string(_id); }

    protected:
        static int _counter;
        int _id;
    };
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
