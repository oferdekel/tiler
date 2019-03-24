////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Statement.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Statement.h"

#include <algorithm>

namespace tiler
{
    double StatementBase::_positionCounter = 0;

    StatementBase::StatementBase() : _position(_positionCounter++)
    {}

    void StatementBase::SetPosition(double Position) 
    { 
        _position = Position; 
    }

    double StatementBase::GetPosition() const 
    { 
        return _position; 
    }

    std::ostream& operator<<(std::ostream& stream, const StatementBase& statement)
    {
        statement.Print(stream);
        return stream;
    }

    LoopStatement::LoopStatement(const Variable& indexVariable, int start, int stop, int step) : _indexVariable(indexVariable), _start(start), _stop(stop), _step(step) 
    {}

    void LoopStatement::Print(std::ostream& stream) const
    {
        stream << "for(int " 
            << GetStatementVariable().GetName()
            << " = " 
            << GetStart() 
            << "; " 
            << GetStatementVariable().GetName()
            << " < " 
            << GetStop()
            << "; "
            << GetStatementVariable().GetName()
            << " += "
            << GetStep()
            << ")    // ForAll statement, position:"
            << GetPosition();
    }

    UsingStatement::UsingStatement(const Variable& matrixVariable, MatrixLayout matrixLayout, float* data) : _matrixVariable(matrixVariable), _matrixLayout(matrixLayout), _data(data)
    {}

    void UsingStatement::Print(std::ostream& stream) const
    {
        stream << "float " 
            << _matrixVariable.GetName() 
            << "["
            << _matrixLayout.Size()
            << "] = {"
            << *_data;

        for(int i=1; i<_matrixLayout.Size(); ++i)
        {
            stream << ", " << _data[i];
        }

        stream << "};    // Using statement, rows:"
            << _matrixLayout.NumRows()
            << ", cols:"
            << _matrixLayout.NumColumns()
            << ", order:"
            << ((_matrixLayout.GetOrder() == MatrixOrder::rowMajor) ? "row" : "column");
    }

    TileStatement::TileStatement(const Variable& tileVariable, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, MatrixLayout tileLayout)
        : _tileVariable(tileVariable), _matrixStatement(matrixStatement), _topStatement(topStatement), _leftStatement(leftStatement), _tileLayout(tileLayout)
    {}

    void TileStatement::Print(std::ostream& stream) const
    {
        stream << "float* "
            << GetStatementVariable().GetName()
            << " = "
            << GetMatrixVariable().GetName()
            << " + ";

        auto matrixLayout = _matrixStatement->GetLayout();

        if(matrixLayout.GetOrder() == MatrixOrder::rowMajor)
        {
            stream << GetTopVariable().GetName()
                << " * "
                << matrixLayout.GetLeadingDimensionSize()
                << " + "
                << GetLeftVariable().GetName()
                << ";";
        }
        else
        {
            stream << GetTopVariable().GetName()
                << " + "
                << GetLeftVariable().GetName()
                << " * "
                << matrixLayout.GetLeadingDimensionSize()
                << ";";
        }

        stream << "    // Tile statement, rows:"
            << _tileLayout.NumRows()
            << ", cols:"
            << _tileLayout.NumColumns()
            << ", order:"
            << ((_tileLayout.GetOrder() == MatrixOrder::rowMajor) ? "row" : "column");
    }

    void TileStatement::SetPositionByDependencies()
    {
        double position = std::max(_topStatement->GetPosition(), _leftStatement->GetPosition());
        SetPosition(position);
    }

    KernelStatement::KernelStatement(MatrixStatementPtr matrixAStatement, MatrixStatementPtr matrixBStatement, MatrixStatementPtr matrixCStatement, KernelType kernel) 
        : _matrixAStatement(matrixAStatement), _matrixBStatement(matrixBStatement), _matrixCStatement(matrixCStatement), _kernel(kernel)
    {}

    void KernelStatement::Print(std::ostream& stream) const
    {
        _kernel(stream, *_matrixAStatement, *_matrixBStatement, *_matrixCStatement);
    }

}
