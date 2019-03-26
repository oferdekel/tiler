////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Statement.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"
#include "MatrixLayout.h"

#include <functional>
#include <iostream>
#include <memory>

namespace tiler
{
    // Base class for nested statements
    class StatementBase
    {
    public:
        // Constructor and virtual destructor
        StatementBase(const Variable& variable);
        virtual ~StatementBase() = default;

        // Returns the variable defined by the statement
        const Variable& GetVariable() const { return _variable; }

        // Virtual function for printing the statement during the forward and backward passes
        virtual void PrintForward(std::ostream& stream) const = 0;
        virtual void PrintBackward(std::ostream& stream) const {}

        // Get and set the statement position
        double GetPosition() const { return _position; }
        void SetPosition(double position);

    private:
        Variable _variable; 
        static double _positionCounter;
        double _position = 0;
    };

    // Prints a statement to a stream by calling its PrintForward() member
    std::ostream& operator<<(std::ostream& stream, const StatementBase& statement);

    // ForAll statements
    class ForAllStatement : public StatementBase
    {
    public:
        // Constructor
        ForAllStatement(const Variable& indexVariable, int start, int stop, int step);

        // Prints the statement
        void PrintForward(std::ostream& stream) const override;
        void PrintBackward(std::ostream& stream) const override;

        int GetStart() const { return _start; }
        int GetStop() const { return _stop; }
        int GetStep() const { return _step; }

    private:
        int _start;
        int _stop;
        int _step;
    };

    // Base class for Matrix statement (Using, Tile)
    class MatrixStatement : public StatementBase
    {
    public:
        // Constructor
        MatrixStatement(const Variable& variable, const MatrixLayout& matrixLayout, bool isOutput);

        // Access the matrix layout
        MatrixLayout& GetLayout() { return _matrixLayout; } 
        const MatrixLayout& GetLayout() const { return _matrixLayout; } 

        // Determines if the matrix is an output matrix
        bool IsOutput() const { return _isOutput; }
        void SetOutput(bool output = true) { _isOutput = output; }

    private:
        MatrixLayout _matrixLayout;
        bool _isOutput;
    };

    // Using statements
    class UsingStatement : public MatrixStatement
    {
    public:
        // Constructor
        UsingStatement(const Variable& matrixVariable, MatrixLayout matrixLayout, bool isOutput, float* data);

        // Prints the statement
        void PrintForward(std::ostream& stream) const override;

    private:
        float* _data;
    };

    // Tile statements
    class TileStatement : public MatrixStatement
    {
    public:
        // Abbreviations
        using StatementPtr = std::shared_ptr<StatementBase>;
        using MatrixStatementPtr = std::shared_ptr<MatrixStatement>;

        // Constructor
        TileStatement(const Variable& tileVariable, MatrixLayout tileLayout, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement);

        // Prints the statement
        void PrintForward(std::ostream& stream) const override;
        void PrintBackward(std::ostream& stream) const override;

        // Sets the position of this statement to be the maximum of its dependencies
        void SetPositionByDependencies();

        // Set the cache flag
        void SetCache(bool cache = true) { _cache = cache; }
        bool IsCached() const { return _cache; }

        // Determines if the tile is transposed with repsect to the original data
        bool IsTransposed() const { return _matrixStatement->GetLayout().GetOrder() != GetLayout().GetOrder(); }

    private:
        MatrixStatementPtr _matrixStatement;
        StatementPtr _topStatement;
        StatementPtr _leftStatement;
        bool _cache = false;
    };

    // Kernel statements
    class KernelStatement : public StatementBase
    {
    public:
        // Abbreviations
        using MatrixStatementPtr = std::shared_ptr<MatrixStatement>;
        using KernelType = std::function<void(std::ostream&, const MatrixStatement&, const MatrixStatement&, const MatrixStatement&)>;

        // Constructor
        KernelStatement(MatrixStatementPtr matrixAStatement, MatrixStatementPtr matrixBStatement, MatrixStatementPtr matrixCStatement, KernelType kernel);

        // Prints the statement
        void PrintForward(std::ostream& stream) const override;

    private:
        MatrixStatementPtr _matrixAStatement;
        MatrixStatementPtr _matrixBStatement;
        MatrixStatementPtr _matrixCStatement;
        KernelType _kernel;
    };
}