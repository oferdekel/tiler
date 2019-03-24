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
#include "Statement.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace tiler
{
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

        template <typename StatementType = StatementBase>
        std::shared_ptr<StatementType> FindStatementByTypeAndVariable(const Variable& variable) const;

        // Prints the nest
        void Print(std::ostream& stream);

    private:
        std::vector<StatementPtr> _statements;
    };

    class NestMutatorBase
    {
    public:
        NestMutatorBase(std::shared_ptr<Nest> nest);

        NestMutatorBase Using(Variable matrixVariable, MatrixLayout matrixLayout, float* data);

        inline auto ForAll(Variable indexVariable, int start, int stop, int step);

        inline auto Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int numRows, int numColumns);

        NestMutatorBase Kernel(const Variable& matrixAVariable, const Variable& matrixBVariable, const Variable& matrixCVariable, KernelStatement::KernelType kernel);

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
        static double _loopCounter;
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

    template <typename StatementType>
    std::shared_ptr<StatementType> Nest::FindStatementByTypeAndVariable(const Variable& variable) const
    {
        for(const auto& statement : _statements)
        {
            if(statement->GetStatementVariable() == variable)
            {
                auto typedStatement = std::dynamic_pointer_cast<StatementType>(statement);
                if(typedStatement != nullptr)
                {
                    return typedStatement;
                }
                else
                {
                    throw std::logic_error("statement that corresponds to variable " + variable.GetName() + " has incorrect type");
                }
            }
        }

        throw std::logic_error("can't find variable " + variable.GetName());
    }

    inline auto NestMutatorBase::ForAll(Variable indexVariable, int start, int stop, int step)
    {
        auto loop = std::make_shared<LoopStatement>(indexVariable, start, stop, step);
        _nest->AddStatement(loop);
        return LoopMutator(_nest, loop);
    }

    inline auto NestMutatorBase::Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int numRows, int numColumns)
    {
        auto matrixStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixVariable);
        auto matrixLayout = matrixStatement->GetLayout();
        
        if(matrixLayout.NumRows() % numRows != 0 || matrixLayout.NumColumns() % numColumns)
        {
            throw std::logic_error("size of tile " + tileVariable.GetName() + " is incompatible with matrix " + matrixStatement->GetStatementVariable().GetName());
        }

        MatrixLayout tileLayout(numRows, numColumns, matrixLayout.GetOrder(), matrixLayout.GetLeadingDimensionSize());

        auto topStatement = _nest->FindStatementByTypeAndVariable(topVariable);
        auto leftStatement = _nest->FindStatementByTypeAndVariable(leftVariable);

        auto tile = std::make_shared<TileStatement>(tileVariable, matrixStatement, topStatement, leftStatement, tileLayout);
        _nest->AddStatement(tile);
        return TileMutator(_nest, tile);
    }

    NestMutatorBase Using(Variable matrixVariable, MatrixLayout matrixLayout, float* data); // some of these args shold be const refeences
}