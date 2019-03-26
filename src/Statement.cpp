////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Statement.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintUtils.h"
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

    std::ostream& operator<<(std::ostream& stream, const StatementBase& statement)
    {
        statement.PrintForward(stream);
        return stream;
    }

    MatrixStatement::MatrixStatement(const Variable& variable, const MatrixLayout& matrixLayout, bool isOutput) : StatementBase(variable), _matrixLayout(matrixLayout), _isOutput(isOutput)
    {}

    ForAllStatement::ForAllStatement(const Variable& indexVariable, int start, int stop, int step) : StatementBase(indexVariable), _start(start), _stop(stop), _step(step) 
    {}

    void ForAllStatement::PrintForward(std::ostream& stream) const
    {
        auto name = GetVariable().GetName();
        stream << Indent;
        PrintFormated(stream, "for(int % = %; % < %; % += %)    // ForAll statement, position:%\n", name, GetStart(), name, GetStop(), name, GetStep(), GetPosition());
        stream << Indent << "{\n";
        IncreaseIndent();
    }

    void ForAllStatement::PrintBackward(std::ostream& stream) const
    {
        DecreaseIndent();
        stream << Indent << "}\n";
    }

    UsingStatement::UsingStatement(const Variable& matrixVariable, MatrixLayout matrixLayout, bool isOutput, float* data) : MatrixStatement(matrixVariable, matrixLayout, isOutput), _data(data)
    {}

    void UsingStatement::PrintForward(std::ostream& stream) const
    {
        auto name = GetVariable().GetName();
        auto layout = GetLayout();
        stream << Indent;
        PrintFormated(stream, "float %[%]", name, GetLayout().Size());

        if(_data != nullptr)
        {
            stream << " = {" << *_data;
            for(int i=1; i<GetLayout().Size(); ++i)
            {
                stream << ", " << _data[i];
            }
            stream << "}";
        }

        PrintFormated(stream, ";    // Using statement, rows:%, cols:%, order:%, output:%\n", layout.NumRows(), layout.NumColumns(), (layout.GetOrder() == MatrixOrder::rowMajor) ? "row" : "column", IsOutput() ? "true" : "false");
    }

    TileStatement::TileStatement(const Variable& tileVariable, MatrixLayout tileLayout, MatrixStatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement)
        : MatrixStatement(tileVariable, tileLayout, matrixStatement->IsOutput()), _matrixStatement(matrixStatement), _topStatement(topStatement), _leftStatement(leftStatement)
    {}

    void TileStatement::PrintForward(std::ostream& stream) const
    {
        auto name = GetVariable().GetName();
        auto matrixLayout = _matrixStatement->GetLayout();
        auto tileLayout = GetLayout();

        // construct string that represents the source location in memory
        std::string source = _matrixStatement->GetVariable().GetName() + " + " + _topStatement->GetVariable().GetName();

        if(matrixLayout.GetOrder() == MatrixOrder::rowMajor)
        {
            source +=  " * " + std::to_string(matrixLayout.GetLeadingDimensionSize()) + " + " + _leftStatement->GetVariable().GetName();
        }
        else
        {
            source += _leftStatement->GetVariable().GetName() + " * " + std::to_string(matrixLayout.GetLeadingDimensionSize());
        }

        stream << Indent;

        if(IsCached())
        { 
            if(tileLayout.GetOrder() == matrixLayout.GetOrder())
            {
                PrintFormated(stream, "Copy(%, %, %, %, %, %);", name, source, tileLayout.GetMinorSize(), tileLayout.GetMajorSize(), tileLayout.GetLeadingDimensionSize(), matrixLayout.GetLeadingDimensionSize());
            }
            else
            {
                PrintFormated(stream, "CopyTranspose(%, %, %, %, %, %);", name, source, tileLayout.GetMinorSize(), tileLayout.GetMajorSize(), tileLayout.GetLeadingDimensionSize(), matrixLayout.GetLeadingDimensionSize());
            }
        }
        else
        {
            PrintFormated(stream, "float* % = %;", name, source);
        }

        PrintFormated(stream, "    // Tile statement, rows:%, columns:%, order:%, cached:%\n", tileLayout.NumRows(), tileLayout.NumColumns(), (tileLayout.GetOrder() == MatrixOrder::rowMajor) ? "row" : "column", IsCached() ? "true" : "false");
    }

    void TileStatement::PrintBackward(std::ostream& stream) const
    {
        if(IsCached() && IsOutput())
        { 
            auto name = GetVariable().GetName();
            auto matrixLayout = _matrixStatement->GetLayout();
            auto tileLayout = GetLayout();

            std::string source = _matrixStatement->GetVariable().GetName() + " + " + _topStatement->GetVariable().GetName();

            if(matrixLayout.GetOrder() == MatrixOrder::rowMajor)
            {
                source +=  " * " + std::to_string(matrixLayout.GetLeadingDimensionSize()) + " + " + _leftStatement->GetVariable().GetName();
            }
            else
            {
                source += _leftStatement->GetVariable().GetName() + " * " + std::to_string(matrixLayout.GetLeadingDimensionSize());
            }

            stream << Indent;

            if(tileLayout.GetOrder() == matrixLayout.GetOrder())
            {
                PrintFormated(stream, "Copy(%, %, %, %, %, %);", source, name, tileLayout.GetMinorSize(), tileLayout.GetMajorSize(), matrixLayout.GetLeadingDimensionSize(), tileLayout.GetLeadingDimensionSize());
            }
            else
            {
                PrintFormated(stream, "CopyTranspose(%, %, %, %, %, %);", source, name, tileLayout.GetMinorSize(), tileLayout.GetMajorSize(), matrixLayout.GetLeadingDimensionSize(), tileLayout.GetLeadingDimensionSize());
            }
            stream << "    // copy output value back from cache\n";
        }
    }

    void TileStatement::SetPositionByDependencies()
    {
        double position = std::max(_topStatement->GetPosition(), _leftStatement->GetPosition());
        SetPosition(position);
    }

    KernelStatement::KernelStatement(MatrixStatementPtr matrixAStatement, MatrixStatementPtr matrixBStatement, MatrixStatementPtr matrixCStatement, KernelType kernel) 
        : StatementBase(Variable()), _matrixAStatement(matrixAStatement), _matrixBStatement(matrixBStatement), _matrixCStatement(matrixCStatement), _kernel(kernel)
    {}

    void KernelStatement::PrintForward(std::ostream& stream) const
    {
        _kernel(stream, *_matrixAStatement, *_matrixBStatement, *_matrixCStatement);
    }

}
