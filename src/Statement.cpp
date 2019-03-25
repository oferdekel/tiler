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

    MatrixStatement::MatrixStatement(const Variable& variable, const MatrixLayout& matrixLayout) : StatementBase(variable), _matrixLayout(matrixLayout)
    {}

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

    UsingStatement::UsingStatement(const Variable& matrixVariable, MatrixLayout matrixLayout, float* data) : MatrixStatement(matrixVariable, matrixLayout), _data(data)
    {}

    void UsingStatement::Print(std::ostream& stream) const
    {
        stream << "float " 
            << GetVariable().GetName() 
            << "["
            << GetLayout().Size()
            << "] = {"
            << *_data;

        for(int i=1; i<GetLayout().Size(); ++i)
        {
            stream << ", " << _data[i];
        }

        stream << "};    // Using statement, rows:"
            << GetLayout().NumRows()
            << ", cols:"
            << GetLayout().NumColumns()
            << ", order:"
            << ((GetLayout().GetOrder() == MatrixOrder::rowMajor) ? "row" : "column")
            << std::endl;
    }

    TileStatement::TileStatement(const Variable& tileVariable, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, MatrixLayout tileLayout)
        : MatrixStatement(tileVariable, tileLayout), _matrixStatement(matrixStatement), _topStatement(topStatement), _leftStatement(leftStatement)
    {}

    void TileStatement::Print(std::ostream& stream) const
    {
        // abreviations
        auto matrixLayout = _matrixStatement->GetLayout();
        auto tileLayout = GetLayout();

        // construct string that represents the source location in memory
        std::string source = _matrixStatement->GetVariable().GetName() + " + " + GetTopVariable().GetName();

        if(matrixLayout.GetOrder() == MatrixOrder::rowMajor)
        {
            source +=  " * " + std::to_string(matrixLayout.GetLeadingDimensionSize()) + " + " + GetLeftVariable().GetName();
        }
        else
        {
            source += GetLeftVariable().GetName() + " * " + std::to_string(matrixLayout.GetLeadingDimensionSize());
        }

        if(IsCached())
        { 
            if(tileLayout.GetOrder() == matrixLayout.GetOrder())
            {
                stream << "copy("
                    << source
                    << ", "
                    << GetVariable().GetName()
                    << ", "
                    << tileLayout.GetMinorSize()
                    << ", "
                    << tileLayout.GetMajorSize()
                    << ", "
                    << matrixLayout.GetLeadingDimensionSize()
                    << ", "
                    << tileLayout.GetLeadingDimensionSize()
                    << ");";
            }
            else
            {
                throw std::logic_error("not yet implemented");
            }
        }
        else
        {
            stream << "float* "
                << GetVariable().GetName()
                << " = "
                << source
                << ";";
        }

        stream << "    // Tile statement, rows:"
            << GetLayout().NumRows()
            << ", cols:"
            << GetLayout().NumColumns()
            << ", order:"
            << ((GetLayout().GetOrder() == MatrixOrder::rowMajor) ? "row" : "column")
            << ", cached:"
            << (IsCached() ? "true" : "false");
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
