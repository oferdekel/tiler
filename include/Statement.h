////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Statement.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"
#include "Matrix.h"

#include <functional>
#include <iostream>
#include <memory>

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

    class LoopStatement : public StatementBase
    {
    public:
        LoopStatement(const Variable& indexVariable, int start, int stop, int step);

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

    class MatrixStatement : public StatementBase
    {
    public:
        virtual const MatrixLayout& GetLayout() const = 0;
    };

    class UsingStatement : public MatrixStatement
    {
    public:
        UsingStatement(const Variable& matrixVariable, MatrixLayout matrixLayout, float* data);

        const Variable& GetStatementVariable() const override { return _matrixVariable; }

        void Print(std::ostream& stream) const override;

        const MatrixLayout& GetLayout() const override { return _matrixLayout; } 

    private:
        Variable _matrixVariable;
        MatrixLayout _matrixLayout;
        float* _data;
    };

    class TileStatement : public MatrixStatement
    {
    public:
        using StatementPtr = std::shared_ptr<StatementBase>;
        using MatrixStatementPtr = std::shared_ptr<MatrixStatement>;

        TileStatement(const Variable& tileVariable, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, MatrixLayout tileLayout);

        const Variable& GetStatementVariable() const override { return _tileVariable; }

        const MatrixLayout& GetLayout() const override { return _tileLayout; } 

        void Print(std::ostream& stream) const override;

        void SetPositionByDependencies();

        Variable GetMatrixVariable() const { return _matrixStatement -> GetStatementVariable(); }
        Variable GetTopVariable() const { return _topStatement -> GetStatementVariable(); }
        Variable GetLeftVariable() const { return _leftStatement -> GetStatementVariable(); }

    private:
        Variable _tileVariable;
        MatrixStatementPtr _matrixStatement;
        StatementPtr _topStatement;
        StatementPtr _leftStatement;
        MatrixLayout _tileLayout;
    };

    class KernelStatement : public StatementBase
    {
    public:
        using MatrixStatementPtr = std::shared_ptr<MatrixStatement>;
        using KernelType = std::function<void(std::ostream&, const MatrixStatement&, const MatrixStatement&, const MatrixStatement&)>;

        KernelStatement(MatrixStatementPtr matrixAStatement, MatrixStatementPtr matrixBStatement, MatrixStatementPtr matrixCStatement, KernelType kernel);

        const Variable& GetStatementVariable() const override { return _matrixCStatement->GetStatementVariable(); }

        void Print(std::ostream& stream) const override;

    private:
        MatrixStatementPtr _matrixAStatement;
        MatrixStatementPtr _matrixBStatement;
        MatrixStatementPtr _matrixCStatement;
        KernelType _kernel;
    };
}