////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Nest.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"
#include "MatrixLayout.h"
#include "Statement.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace tiler
{
    // Represents a loop nest 
    class Nest
    {
    public:
        using StatementPtr = std::shared_ptr<StatementBase>;
        using UsingStatementPtr = std::shared_ptr<UsingStatement>;

        // Adds an element to the nest
        void AddStatement(StatementPtr nestStatement);

        // Returns the numer of elements defined in the nest
        int Size() const;

        // Finds a statement that matches a type and variable name
        template <typename StatementType = StatementBase>
        std::shared_ptr<StatementType> FindStatementByTypeAndVariable(const Variable& variable) const;

        // Prints C++ code that implements the nest
        void Print(std::ostream& stream);

    private:
        std::vector<StatementPtr> _statements;
    };

    // Appends statements to a loop nest, serves as the base class for statement modifiers
    class NestStatementAppender
    {
    public:
        // Constructor
        NestStatementAppender(std::shared_ptr<Nest> nest);

        // Appends a Using statement
        NestStatementAppender Using(Variable matrixVariable, MatrixLayout matrixLayout, bool isOutput, float* data);

        // Appends a ForAll statement
        inline auto ForAll(Variable indexVariable, int start, int stop, int step);

        // Appends a Tile statement
        inline auto Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int numRows, int numColumns);

        // Appends a Kernel statement
        NestStatementAppender Kernel(const Variable& matrixAVariable, const Variable& matrixBVariable, const Variable& matrixCVariable, KernelStatement::KernelType kernel);

        // Prints the underlying nest
        void Print(std::ostream& stream) const;

    protected:
        std::shared_ptr<Nest> _nest;
    };

    // Modifies ForAll statements, and appends new statements to a loop nest
    class ForAllStatementModifier : public NestStatementAppender
    {
    public:
        // Constructor
        ForAllStatementModifier(std::shared_ptr<Nest> nest, std::shared_ptr<ForAllStatement> loop);

        // Modifies the position of the underlying ForAll loop
        ForAllStatementModifier Position(double Position);

    private:
        static double _loopCounter;
        std::shared_ptr<ForAllStatement> _loop;
    };

    // Modifies Tile statements, and appends new statements to a loop nest
    class TileStatementModifier : public NestStatementAppender
    {
    public:
        // Constructor
        TileStatementModifier(std::shared_ptr<Nest> nest, std::shared_ptr<TileStatement> tile);

        // Tells the Tile statement to cache the tile 
        NestStatementAppender Cache(MatrixOrder order);

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
            if(statement->GetVariable() == variable)
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

    inline auto NestStatementAppender::ForAll(Variable indexVariable, int start, int stop, int step)
    {
        auto loop = std::make_shared<ForAllStatement>(indexVariable, start, stop, step);
        _nest->AddStatement(loop);
        return ForAllStatementModifier(_nest, loop);
    }

    inline auto NestStatementAppender::Tile(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int numRows, int numColumns)
    {
        auto matrixStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixVariable);
        auto matrixLayout = matrixStatement->GetLayout();
        
        if(matrixLayout.NumRows() % numRows != 0 || matrixLayout.NumColumns() % numColumns)
        {
            throw std::logic_error("size of tile " + tileVariable.GetName() + " is incompatible with matrix " + matrixStatement->GetVariable().GetName());
        }

        MatrixLayout tileLayout(numRows, numColumns, matrixLayout.GetOrder(), matrixLayout.GetLeadingDimensionSize());

        auto topStatement = _nest->FindStatementByTypeAndVariable(topVariable);
        auto leftStatement = _nest->FindStatementByTypeAndVariable(leftVariable);

        auto tile = std::make_shared<TileStatement>(tileVariable, tileLayout, matrixStatement, topStatement, leftStatement);
        _nest->AddStatement(tile);
        return TileStatementModifier(_nest, tile);
    }

    NestStatementAppender MakeNest(); 
}