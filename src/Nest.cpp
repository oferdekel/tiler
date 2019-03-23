////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Nest.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nest.h"
#include "IndentedOutputStream.h"

#include <algorithm>
#include <unordered_set>

namespace tiler
{
    template<typename IsType, typename OrigType>
    bool IsPointerTo(const OrigType& pointer)
    {
        return (std::dynamic_pointer_cast<IsType>(pointer) != nullptr);
    }

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

    TileStatement::TileStatement(Variable tileVariable, StatementPtr matrixStatement, StatementPtr topStatement, StatementPtr leftStatement, int height, int width)
        : _tileVariable(tileVariable), _matrixStatement(matrixStatement), _topStatement(topStatement), _leftStatement(leftStatement), _height(height), _width(width) 
    {}

    void TileStatement::Print(std::ostream& stream) const
    {
        stream << "float* "
            << GetStatementVariable().GetName()
            << " = Tile("
            << GetMatrixVariable().GetName()
            << ", "
            << GetTopVariable().GetName()
            << ", "
            << GetLeftVariable().GetName()
            << ", "
            << GetHeight()
            << ", "
            << GetWidth()
            <<");";
    }

    void TileStatement::SetPositionByDependencies()
    {
        double position = std::max(_topStatement->GetPosition(), _leftStatement->GetPosition());
        SetPosition(position);
    }

    void Nest::AddStatement(Nest::StatementPtr nestStatement)
    {
        nestStatement->SetPosition(Size());
        _statements.push_back(nestStatement);
    }

    int Nest::Size() const 
    { 
        return (int)_statements.size(); 
    }

    StatementBase& Nest::Back() 
    { 
        return *(_statements.back()); 
    }

    Nest::StatementPtr Nest::FindStatementByVariable(const Variable& variable) const
    {
        for(const auto& statement : _statements)
        {
            if(statement->GetStatementVariable() == variable)
            {
                return statement;
            }
        }

        throw std::logic_error("can't find variable " + variable.GetName());
    }

    void Nest::Print(std::ostream& stream)
    {
        IndentedOutputStream indentedStream(stream);

        // pre-sort pass
        for(const auto& statement : _statements)
        {
            auto tileStatement = std::dynamic_pointer_cast<TileStatement>(statement);
            if(tileStatement != nullptr)
            {
                tileStatement->SetPositionByDependencies();
            }
        }

        // sort the statements
        auto comparer = [](const StatementPtr& a, const StatementPtr& b) 
        {
            // using statements are always before other statements
            if (IsPointerTo<UsingStatement>(a) && !IsPointerTo<UsingStatement>(b))
            {
                return true;
            }

            if (IsPointerTo<UsingStatement>(b) && !IsPointerTo<UsingStatement>(a))
            {
                return false;
            }

            // ties are broken such that tile statements are last
            if (a->GetPosition() == b->GetPosition())
            {
                if(IsPointerTo<LoopStatement>(a) && IsPointerTo<TileStatement>(b))
                {
                    return true;
                }

                if(IsPointerTo<LoopStatement>(b) && IsPointerTo<TileStatement>(a))
                {
                    return false;
                }
            }

            // otherwise, sort by position
            return a->GetPosition() < b->GetPosition();
        };
        std::sort(_statements.begin(), _statements.end(), comparer);

        // post-sort forward pass
        for(const auto& statement : _statements)
        {
            indentedStream << *statement;

            if(IsPointerTo<LoopStatement>(statement))
            {
                indentedStream << " {";
                indentedStream.IncreaseIndent();
            }

            indentedStream << endl;
        }

        // backwards pass
        std::reverse(_statements.begin(), _statements.end());
        for(const auto& statement : _statements)
        {
            if(IsPointerTo<LoopStatement>(statement))
            {
                indentedStream.DecreaseIndent();
                indentedStream << "}" << endl;
            }
        }
    }

    NestMutatorBase::NestMutatorBase(std::shared_ptr<Nest> nest) : _nest(nest) 
    {}

    NestMutatorBase NestMutatorBase::Using(Matrix matrix)
    {
        auto statement = std::make_shared<UsingStatement>(matrix);
        _nest->AddStatement(statement);
        return *this;
    }

    void NestMutatorBase::Print(std::ostream& stream) const
    { 
        return _nest->Print(stream); 
    }

    LoopMutator::LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopStatement> loop) : NestMutatorBase(nest), _loop(loop) 
    {}

    LoopMutator LoopMutator::Position(double Position) 
    { 
        _loop->SetPosition(Position); 
        return *this; 
    }

    TileMutator::TileMutator(std::shared_ptr<Nest> nest, std::shared_ptr<TileStatement> tile) : NestMutatorBase(nest), _tile(tile) 
    {}

}