////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Nest.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <stdexcept>

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

    protected:
        static std::default_random_engine _generator;
        long _uid;
    };

    // Generic element of a nest
    class NestElement
    {
    public:
        NestElement(Variable variable, double order);
        virtual ~NestElement() = default;

        Variable GetVariable() const;

        void SetOrder(double order);

        double GetOrder() const;

    private:
        double _order;
        Variable _variable; 
    };

    class LoopElement : public NestElement
    {
    public:
        LoopElement(Variable index, int start, int stop, int step, double order);

    private:
        int _start;
        int _stop;
        int _step;
    };

    // Datastructure that stores the variables that make up the nested loop
    class Nest
    {
    public:
        // Adds an element to the nest
        void AddElement(std::shared_ptr<NestElement> nestElement);

        // Returns the numer of elements defined in the nest
        int Size() const;

        // Returns a reference to the last element in the nest
        NestElement& Back();

        // Checks if a variable was previously defined in the nest
        bool IsDefined(Variable variable) const;

    private:
        std::vector<std::shared_ptr<NestElement>> _elements;
    };

    
    class NestMutator
    {
    public:
        NestMutator(std::shared_ptr<Nest> nest);

        inline auto ForAll(Variable index, int start, int stop, int step);

        NestMutator Order(double order);

        int NestSize() const;

    protected:
        std::shared_ptr<Nest> _nest;
    };

    class LoopMutator : public NestMutator
    {
    public:
        LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopElement> loop);

    private:
        std::shared_ptr<LoopElement> _loop;
    };

    template <typename... T> 
    LoopMutator ForAll(T... t);

    //
    //
    //

    inline auto NestMutator::ForAll(Variable index, int start, int stop, int step)
    {
        auto loop = std::make_shared<LoopElement>(index, start, stop, step, 0.0);
        _nest->AddElement(loop);
        return LoopMutator(_nest, loop);
    }

    template <typename... T> 
    LoopMutator ForAll(T... t)
    {
        auto nest = std::make_shared<Nest>();
        NestMutator nestMutator(nest);
        return nestMutator.ForAll(t...);
    }
}