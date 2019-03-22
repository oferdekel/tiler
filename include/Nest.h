////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Nest.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"
#include "Matrix.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace tiler
{
    // Generic element of a nest
    class NestStatementBase
    {
    public:
        NestStatementBase(Variable declaredVariable);
        virtual ~NestStatementBase() = default;

        Variable GetDeclaredVariable() const;

        void SetPosition(double position);

        double GetPosition() const;

    private:
        double _position = 0;
        Variable _declaredVariable; 
    };

    class UsingDeclaration
    {
    public:
        UsingDeclaration(Matrix matrix);

        Matrix GetMatrix() const { return _matrix; } 

    private:
        Matrix _matrix;
    };

    // Prints a loop declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const UsingDeclaration& usingStatement);

    class LoopStatement : public NestStatementBase
    {
    public:
        LoopStatement(Variable indexVariable, int start, int stop, int step);

        int GetStart() const { return _start; }
        int GetStop() const { return _stop; }
        int GetStep() const { return _step; }

    private:
        int _start;
        int _stop;
        int _step;
    };

    // Prints a loop declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const LoopStatement& loopStatement);

    class TileStatement : public NestStatementBase
    {
    public:
        TileStatement(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width);

        Variable GetMatrixVariable() const { return _matrixVariable; }
        Variable GetTopVariable() const { return _topVariable; }
        Variable GetLeftVariable() const { return _leftVariable; }
        int GetHeight() const { return _height; }
        int GetWidth() const { return _width; }        

    private:
        Variable _matrixVariable;
        Variable _topVariable;
        Variable _leftVariable;
        int _height;
        int _width;
    };

    // Prints a tile declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const TileStatement& tileStatement);

    // Datastructure that stores the variables that make up the nested loop
    class Nest
    {
    public:
        using NestStatementPtr = std::shared_ptr<NestStatementBase>;
        using UsingDeclarationPtr = std::shared_ptr<UsingDeclaration>;

        // Adds an element to the nest
        void AddStatement(NestStatementPtr nestStatement);

        // Adds a using declaration
        void AddUsingDeclaration(UsingDeclarationPtr usingDeclaration);

        // Returns the numer of elements defined in the nest
        int Size() const;

        // Returns a reference to the last element in the nest
        NestStatementBase& Back();

        // Prints the nest
        void Print(std::ostream& stream) const;

    private:

        std::vector<NestStatementPtr> GetSortedStatementsCopy() const;
        void VerifyStatementPositions(std::vector<NestStatementPtr> sortedStatements) const;

        std::vector<UsingDeclarationPtr> _usingDeclarations;
        std::vector<NestStatementPtr> _statements;
    };

    class NestDeclarer
    {
    public:
        NestDeclarer(std::shared_ptr<Nest> nest);

        NestDeclarer Using(Matrix matrix);

        inline auto ForAll(Variable indexVariable, int start, int stop, int step);

        inline auto Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width);

        NestDeclarer Position(double Position);

        int NestSize() const;

        void Print(std::ostream& stream) const;

    protected:
        std::shared_ptr<Nest> _nest;
    };

    class LoopMutator : public NestDeclarer
    {
    public:
        LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopStatement> loop);

    private:
        std::shared_ptr<LoopStatement> _loop;
    };

    class TileMutator : public NestDeclarer
    {
    public:
        TileMutator(std::shared_ptr<Nest> nest, std::shared_ptr<TileStatement> tile);

    private:
        std::shared_ptr<TileStatement> _tile;
    };

    //
    //
    //

    inline auto NestDeclarer::ForAll(Variable indexVariable, int start, int stop, int step)
    {
        auto loop = std::make_shared<LoopStatement>(indexVariable, start, stop, step);
        _nest->AddStatement(loop);
        return LoopMutator(_nest, loop);
    }

    inline auto NestDeclarer::Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width)
    {
        auto tile = std::make_shared<TileStatement>(tileVariable, matrixVariable, topVariable, leftVariable, height, width);
        _nest->AddStatement(tile);
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