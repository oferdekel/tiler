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
        StatementBase(const Variable& variable);
        virtual ~StatementBase() = default;

        const Variable& GetVariable() const { return _variable; }

        virtual void Print(std::ostream& stream) const = 0;

        void SetPosition(double position);

        double GetPosition() const;

    private:
        Variable _variable; 
        static double _positionCounter;
        double _position = 0;
    };

    // Prints a loop declaration to a stream
    std::ostream& operator<<(std::ostream& stream, const StatementBase& statement);

    class LoopStatement : public StatementBase
    {
    public:
        LoopStatement(const Variable& indexVariable, int start, int stop, int step);

        void Print(std::ostream& stream) const override;

        int GetStart() const { return _start; }
        int GetStop() const { return _stop; }
        int GetStep() const { return _step; }

    private:
        int _start;
        int _stop;
        int _step;
    };

    class MatrixStatement : public StatementBase
    {
    public:
        MatrixStatement(const Variable& variable, const MatrixLayout& matrixLayout);

        MatrixLayout& GetLayout() { return _matrixLayout; } 
        const MatrixLayout& GetLayout() const { return _matrixLayout; } 

    private:
        MatrixLayout _matrixLayout;
    };

    class UsingStatement : public MatrixStatement
    {
    public:
        UsingStatement(const Variable& matrixVariable, MatrixLayout matrixLayout, float* data);

        void Print(std::ostream& stream) const override;

    private:
        float* _data;
    };

    class TileStatement : public MatrixStatement
    {
    public:
        using StatementPtr = std::shared_ptr<StatementBase>;
        using MatrixStatementPtr = std::shared_ptr<MatrixStatement>;

        TileStatement(const Variable& tileVariable, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, MatrixLayout tileLayout);

        void Print(std::ostream& stream) const override;

        Variable GetMatrixVariable() const { return _matrixStatement -> GetVariable(); }
        Variable GetTopVariable() const { return _topStatement -> GetVariable(); }
        Variable GetLeftVariable() const { return _leftStatement -> GetVariable(); }

        void SetPositionByDependencies();

        void SetCache(bool cache) { _cache = cache; }
        bool IsCached() const { return _cache; }

    private:
        MatrixStatementPtr _matrixStatement;
        StatementPtr _topStatement;
        StatementPtr _leftStatement;
        bool _cache = false;
    };

    class KernelStatement : public StatementBase
    {
    public:
        using MatrixStatementPtr = std::shared_ptr<MatrixStatement>;
        using KernelType = std::function<void(std::ostream&, const MatrixStatement&, const MatrixStatement&, const MatrixStatement&)>;

        KernelStatement(MatrixStatementPtr matrixAStatement, MatrixStatementPtr matrixBStatement, MatrixStatementPtr matrixCStatement, KernelType kernel);

        void Print(std::ostream& stream) const override;

    private:
        MatrixStatementPtr _matrixAStatement;
        MatrixStatementPtr _matrixBStatement;
        MatrixStatementPtr _matrixCStatement;
        KernelType _kernel;
    };
}