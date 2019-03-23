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
    class StatementBase
    {
    public:
        StatementBase();
        virtual ~StatementBase() = default;

        virtual const Variable& GetStatementVariable() const = 0; 

        virtual void Print(std::ostream& stream) const = 0;

        void SetPosition(double position);

        double GetPosition() const;

    private:
        static double _positionCounter;
        double _position = 0;
    };

    // Prints a loop declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const StatementBase& statement);

    class UsingStatement : public StatementBase
    {
    public:
        UsingStatement(Matrix matrix);

        const Variable& GetStatementVariable() const override { return _matrixVariable; }

        void Print(std::ostream& stream) const override;

        const Matrix& GetMatrix() const { return _matrixVariable; } 

    private:
        Matrix _matrixVariable;
    };

    class LoopStatement : public StatementBase
    {
    public:
        LoopStatement(Variable indexVariable, int start, int stop, int step);

        const Variable& GetStatementVariable() const override { return _indexVariable; }

        void Print(std::ostream& stream) const override;

        int GetStart() const { return _start; }
        int GetStop() const { return _stop; }
        int GetStep() const { return _step; }

    private:
        Variable _indexVariable; 
        int _start;
        int _stop;
        int _step;
    };

    // Prints a loop declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const LoopStatement& loopStatement);

    class TileStatement : public StatementBase
    {
    public:
        using StatementPtr = std::shared_ptr<StatementBase>;

        TileStatement(Variable tileVariable, StatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, int height, int width);

        const Variable& GetStatementVariable() const override { return _tileVariable; }

        void Print(std::ostream& stream) const override;

        void SetPositionByDependencies();

        Variable GetMatrixVariable() const { return _matrixStatement -> GetStatementVariable(); }
        Variable GetTopVariable() const { return _topStatement -> GetStatementVariable(); }
        Variable GetLeftVariable() const { return _leftStatement -> GetStatementVariable(); }

        int GetHeight() const { return _height; }
        int GetWidth() const { return _width; }

    private:
        Variable _tileVariable;
        StatementPtr _matrixStatement;
        StatementPtr _topStatement;
        StatementPtr _leftStatement;
        int _height;
        int _width;
    };

    // Prints a tile declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const TileStatement& tileStatement);

    // Datastructure that stores the variables that make up the nested loop
    class Nest
    {
    public:
        using StatementPtr = std::shared_ptr<StatementBase>;
        using UsingStatementPtr = std::shared_ptr<UsingStatement>;

        // Adds an element to the nest
        void AddStatement(StatementPtr nestStatement);

        // Returns the numer of elements defined in the nest
        int Size() const;

        // Returns a reference to the last element in the nest
        StatementBase& Back();

        StatementPtr FindStatementByVariable(const Variable& variable) const;

        // Prints the nest
        void Print(std::ostream& stream);

    private:
        std::vector<StatementPtr> _statements;
    };

    class NestMutatorBase
    {
    public:
        NestMutatorBase(std::shared_ptr<Nest> nest);

        NestMutatorBase Using(Matrix matrix);

        inline auto ForAll(Variable indexVariable, int start, int stop, int step);

        inline auto Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width);

        void Print(std::ostream& stream) const;

    protected:
        std::shared_ptr<Nest> _nest;
    };

    class LoopMutator : public NestMutatorBase
    {
    public:
        LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopStatement> loop);

        LoopMutator Position(double Position);

    private:
        std::shared_ptr<LoopStatement> _loop;
    };

    class TileMutator : public NestMutatorBase
    {
    public:
        TileMutator(std::shared_ptr<Nest> nest, std::shared_ptr<TileStatement> tile);

    private:
        std::shared_ptr<TileStatement> _tile;
    };

    //
    //
    //

    inline auto NestMutatorBase::ForAll(Variable indexVariable, int start, int stop, int step)
    {
        auto loop = std::make_shared<LoopStatement>(indexVariable, start, stop, step);
        _nest->AddStatement(loop);
        return LoopMutator(_nest, loop);
    }

    inline auto NestMutatorBase::Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width)
    {
        auto matrixStatement = _nest->FindStatementByVariable(matrixVariable);
        auto topStatement = _nest->FindStatementByVariable(topVariable);
        auto leftStatement = _nest->FindStatementByVariable(leftVariable);

        auto tile = std::make_shared<TileStatement>(tileVariable, matrixStatement, topStatement, leftStatement, height, width);
        _nest->AddStatement(tile);
        return TileMutator(_nest, tile);
    }

    template <typename... T> 
    NestMutatorBase Using(T... t)
    {
        auto nest = std::make_shared<Nest>();
        NestMutatorBase NestMutatorBase(nest);
        return NestMutatorBase.Using(t...);
    }

    template <typename... T> 
    LoopMutator ForAll(T... t)
    {
        auto nest = std::make_shared<Nest>();
        NestMutatorBase NestMutatorBase(nest);
        return NestMutatorBase.ForAll(t...);
    }
}