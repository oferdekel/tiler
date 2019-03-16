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
#include <stdexcept>
#include <vector>

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

    // Generic element of a nest
    class NestDeclaration
    {
    public:
        NestDeclaration(Variable declaredVariable);
        virtual ~NestDeclaration() = default;

        Variable GetDeclaredVariable() const;

        void SetPosition(double position);

        double GetPosition() const;

    private:
        double _position = 0;
        Variable _declaredVariable; 
    };

    class LoopDeclaration : public NestDeclaration
    {
    public:
        LoopDeclaration(Variable index, int start, int stop, int step);

        int GetStart() const { return _start; }
        int GetStop() const { return _stop; }
        int GetStep() const { return _step; }

    private:
        int _start;
        int _stop;
        int _step;
    };


    // Prints a loop declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const LoopDeclaration& loopDeclaration);

    class TileDeclaration : public NestDeclaration
    {
    public:
        TileDeclaration(Variable tileVariable, Variable matrix, Variable top, Variable left, int height, int width);

        Variable GetMatrix() const { return _matrix; }
        Variable GetTop() const { return _top; }
        Variable GetLeft() const { return _left; }
        int GetHeight() const { return _height; }
        int GetWidth() const { return _width; }        

    private:
        Variable _matrix;
        Variable _top;
        Variable _left;
        int _height;
        int _width;
    };

    // Prints a tile declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const TileDeclaration& loopDeclaration);

    // Datastructure that stores the variables that make up the nested loop
    class Nest
    {
    public:
        using NestDeclarationPtr = std::shared_ptr<NestDeclaration>;

        // Adds an element to the nest
        void AddDeclaration(NestDeclarationPtr NestDeclaration);

        // Returns the numer of elements defined in the nest
        int Size() const;

        // Returns a reference to the last element in the nest
        NestDeclaration& Back();

        // Checks if a variable was previously defined in the nest
        bool IsDeclared(Variable variable) const;

        // Prints the nest
        void Print(std::ostream& stream) const;

    private:
        std::vector<NestDeclarationPtr> _declarations;
    };

    class NestDeclarer
    {
    public:
        NestDeclarer(std::shared_ptr<Nest> nest);

        NestDeclarer Using(Variable matrix);

        inline auto ForAll(Variable index, int start, int stop, int step);

        inline auto Tile(Variable tileVariable, Variable matrix, Variable top, Variable left, int height, int width);

        NestDeclarer Position(double Position);

        int NestSize() const;

        void Print(std::ostream& stream) const;

    protected:
        std::shared_ptr<Nest> _nest;
    };

    class LoopMutator : public NestDeclarer
    {
    public:
        LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopDeclaration> loop);

    private:
        std::shared_ptr<LoopDeclaration> _loop;
    };

    class TileMutator : public NestDeclarer
    {
    public:
        TileMutator(std::shared_ptr<Nest> nest, std::shared_ptr<TileDeclaration> tile);

    private:
        std::shared_ptr<TileDeclaration> _tile;
    };

    //
    //
    //

    inline auto NestDeclarer::ForAll(Variable index, int start, int stop, int step)
    {
        auto loop = std::make_shared<LoopDeclaration>(index, start, stop, step);
        _nest->AddDeclaration(loop);
        return LoopMutator(_nest, loop);
    }

    inline auto NestDeclarer::Tile(Variable tileVariable, Variable matrix, Variable top, Variable left, int height, int width)
    {
        auto tile = std::make_shared<TileDeclaration>(tileVariable, matrix, top, left, height, width);
        _nest->AddDeclaration(tile);
        return TileMutator(_nest, tile);
    }

    template <typename... T> 
    NestDeclarer Using(T... t)
    {
        auto nest = std::make_shared<Nest>();
        NestDeclarer NestDeclarer(nest);
        return NestDeclarer.Using(t...);
    }

    template <typename... T> 
    LoopMutator ForAll(T... t)
    {
        auto nest = std::make_shared<Nest>();
        NestDeclarer NestDeclarer(nest);
        return NestDeclarer.ForAll(t...);
    }
}