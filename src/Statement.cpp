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

    LoopStatement::LoopStatement(Variable indexVariable, int start, int stop, int step) : _indexVariable(indexVariable), _start(start), _stop(stop), _step(step) 
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
            << ")";
    }

    UsingStatement::UsingStatement(Matrix matrixVariable) : _matrixVariable(matrixVariable)
    {}

    void UsingStatement::Print(std::ostream& stream) const
    {
        stream << "float " 
            << _matrixVariable.GetName() 
            << "["
            << _matrixVariable.Size()
            << "] = ";
        _matrixVariable.PrintData(stream);
        stream << ";";
    }

    TileStatement::TileStatement(Variable tileVariable, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, MatrixLayout tileLayout)
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
        _kernel(stream, _matrixAStatement->GetLayout(), _matrixBStatement->GetLayout(), _matrixCStatement->GetLayout());
    }

}
