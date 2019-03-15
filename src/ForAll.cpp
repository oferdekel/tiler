////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     ForAll.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForAll.h"
#include "IndentedOutputStream.h"

namespace tiler
{
    ForAllNest& ForAllNest::ForAll(Index index, int start, int stop, int step)
    {
        _forLoopInfo.push_back({index, start, stop, step, (double)_forLoopInfo.size()});
        return *this;
    }

    ForAllNest& ForAllNest::Order(float order)
    {
        _forLoopInfo.back().order = order;
        return *this;
    }

    void ForAllNest::Print(std::ostream& stream) const
    {
        IndentedOutputStream s(stream);
        s << "test";
    }
}