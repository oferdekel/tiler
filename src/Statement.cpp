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

    StatementBase::StatementBase(const Variable& variable) : _variable(variable), _position(_positionCounter++)
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

    LoopStatement::LoopStatement(const Variable& indexVariable, int start, int stop, int step) : StatementBase(indexVariable), _start(start), _stop(stop), _step(step) 
    {}

    void LoopStatement::Print(std::ostream& stream) const
    {
        stream << "for(int " 
            << GetVariable().GetName()
            << " = " 
            << GetStart() 
            << "; " 
            << GetVariable().GetName()
            << " < " 
            << GetStop()
            << "; "
            << GetVariable().GetName()
            << " += "
            << GetStep()
            << ")    // ForAll statement, position:"
            << GetPosition();
    }

    UsingStatement::UsingStatement(const Variable& matrixVariable, MatrixLayout matrixLayout, float* data) : MatrixStatement(matrixVariable), _matrixLayout(matrixLayout), _data(data)
    {}

    void UsingStatement::Print(std::ostream& stream) const
    {
        stream << "float " 
            << GetVariable().GetName() 
            << "["
            << GetLayout().Size()
            << "] = {"
            << *_data;

        for(int i=1; i<_matrixLayout.Size(); ++i)
        {
            stream << ", " << _data[i];
        }

        stream << "};    // Using statement, rows:"
            << GetLayout().NumRows()
            << ", cols:"
            << GetLayout().NumColumns()
            << ", order:"
            << ((_matrixLayout.GetOrder() == MatrixOrder::rowMajor) ? "row" : "column");
    }

    TileStatement::TileStatement(const Variable& tileVariable, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, MatrixLayout tileLayout)
        : MatrixStatement(tileVariable), _matrixStatement(matrixStatement), _topStatement(topStatement), _leftStatement(leftStatement), _tileLayout(tileLayout)
    {}

    void TileStatement::Print(std::ostream& stream) const
    {
        stream << "float* "
            << GetVariable().GetName()
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
            << GetLayout().NumRows()
            << ", cols:"
            << GetLayout().NumColumns()
            << ", order:"
            << ((GetLayout().GetOrder() == MatrixOrder::rowMajor) ? "row" : "column");
    }

    void TileStatement::SetPositionByDependencies()
    {
        double position = std::max(_topStatement->GetPosition(), _leftStatement->GetPosition());
        SetPosition(position);
    }

    KernelStatement::KernelStatement(MatrixStatementPtr matrixAStatement, MatrixStatementPtr matrixBStatement, MatrixStatementPtr matrixCStatement, KernelType kernel) 
        : StatementBase(Variable()), _matrixAStatement(matrixAStatement), _matrixBStatement(matrixBStatement), _matrixCStatement(matrixCStatement), _kernel(kernel)
    {}

    void KernelStatement::Print(std::ostream& stream) const
    {
        _kernel(stream, *_matrixAStatement, *_matrixBStatement, *_matrixCStatement);
    }

}
