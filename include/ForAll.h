////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     ForAll.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <vector>

namespace tiler
{
    // An index for a forall loop
    struct Index{};

    struct ForLoopInfo
    {
        Index index;
        int start;
        int stop;
        int step;
        double order;
    };

    class ForAllNest
    {
    public:
        ForAllNest& ForAll(Index x, int start, int stop, int step);
        ForAllNest& Order(float order);
        void Print(std::ostream& stream) const;

    private:
        std::vector<ForLoopInfo> _forLoopInfo;
    };

    template <typename... T> 
    ForAllNest ForAll(T... t);

    //
    //
    //

    template <typename... T> 
    ForAllNest ForAll(T... t)
    {
        ForAllNest f;
        f.ForAll(t...);
        return f;
    }
}