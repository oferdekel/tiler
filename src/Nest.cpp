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

    double NestStatementBase::_positionCounter = 0;

    NestStatementBase::NestStatementBase() : _position(_positionCounter++)
    {}

    void NestStatementBase::SetPosition(double Position) 
    { 
        _position = Position; 
    }

    double NestStatementBase::GetPosition() const 
    { 
        return _position; 
    }

    std::ostream& operator<<(std::ostream& stream, const NestStatementBase& statement)
    {
        statement.Print(stream);
        return stream;
    }

    UsingDeclaration::UsingDeclaration(Matrix matrixVariable) : _matrixVariable(matrixVariable)
    {}

    void UsingDeclaration::Print(std::ostream& stream) const
    {
        stream << "float " 
            << GetMatrix().GetName() 
            << "["
            << GetMatrix().Size()
            << "] = ";
        GetMatrix().PrintData(stream);
        stream << ";";
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

    TileStatement::TileStatement(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width) 
        : _tileVariable(tileVariable), _matrixVariable(matrixVariable), _topVariable(topVariable), _leftVariable(leftVariable), _height(height), _width(width) 
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

    void Nest::AddStatement(Nest::NestStatementPtr nestStatement)
    {
        nestStatement->SetPosition(Size());
        _statements.push_back(nestStatement);
    }

    int Nest::Size() const 
    { 
        return (int)_statements.size(); 
    }

    NestStatementBase& Nest::Back() 
    { 
        return *(_statements.back()); 
    }

    void Nest::Print(std::ostream& stream) const
    {
        IndentedOutputStream indentedStream(stream);

        // sort the declaratins based on the order
        auto copy = GetSortedStatementsCopy();
        VerifyStatementPositions(copy);

        for(const auto& statement : copy)
        {
            indentedStream << *statement << endl;

            auto loopStatement = std::dynamic_pointer_cast<LoopStatement>(statement);
            if(loopStatement != nullptr)
            {
                indentedStream << " {" << endl;
                indentedStream.IncreaseIndent();
            }
        }

        std::reverse(copy.begin(), copy.end());
        for(const auto& statement : copy)
        {
            auto loopStatement = std::dynamic_pointer_cast<LoopStatement>(statement);
            if(loopStatement != nullptr)
            {
                indentedStream.DecreaseIndent();
                indentedStream << "}" << endl;
            }
        }
    }

    std::vector<Nest::NestStatementPtr> Nest::GetSortedStatementsCopy() const
    {
        std::vector<NestStatementPtr> copy(_statements);
        auto comparer = [](const NestStatementPtr& a, const NestStatementPtr& b) 
        {
            if(std::dynamic_pointer_cast<UsingDeclaration>(a) != nullptr && std::dynamic_pointer_cast<UsingDeclaration>(b) == nullptr)
            {
                return false;
            }
            return a->GetPosition() < b->GetPosition();
        };

        std::sort(copy.begin(), copy.end(), comparer);
        return copy;
    }

    void Nest::VerifyStatementPositions(std::vector<Nest::NestStatementPtr> sortedStatements) const
    {
        std::unordered_set<Variable> definedVariables;
        for(const auto& declaration : sortedStatements)
        {
            auto variable = declaration->GetStatementVariable();
            if(definedVariables.find(variable) != definedVariables.end())
            {
                throw std::logic_error("variable " + std::to_string(variable.GetID()) + " multiply defined");
            }

            auto tileStatement = std::dynamic_pointer_cast<TileStatement>(declaration);
            if(tileStatement != nullptr)
            {
                if(definedVariables.find(tileStatement->GetMatrixVariable()) == definedVariables.end())
                {
                    throw std::logic_error("tile variable " + std::to_string(variable.GetID()) + " requires previous declaration of matrix variable " + std::to_string(tileStatement->GetMatrixVariable().GetID()));
                }

                if(definedVariables.find(tileStatement->GetTopVariable()) == definedVariables.end())
                {
                    throw std::logic_error("tile variable " + std::to_string(variable.GetID()) + " requires previous declaration of top variable " + std::to_string(tileStatement->GetTopVariable().GetID()));
                }

                if(definedVariables.find(tileStatement->GetLeftVariable()) == definedVariables.end())
                {
                    throw std::logic_error("tile variable " + std::to_string(variable.GetID()) + " requires previous declaration of left variable " + std::to_string(tileStatement->GetLeftVariable().GetID()));
                }
            }

            definedVariables.insert(variable);
        }
    }

    NestDeclarer::NestDeclarer(std::shared_ptr<Nest> nest) : _nest(nest) 
    {}

    NestDeclarer NestDeclarer::Using(Matrix matrix)
    {
        auto statement = std::make_shared<UsingDeclaration>(matrix);
        _nest->AddStatement(statement);
        return NestDeclarer(_nest);
    }

    NestDeclarer NestDeclarer::Position(double Position) 
    { 
        _nest->Back().SetPosition(Position); return NestDeclarer(_nest); 
    }

    int NestDeclarer::NestSize() const 
    { 
        return _nest->Size(); 
    }

    void NestDeclarer::Print(std::ostream& stream) const
    { 
        return _nest->Print(stream); 
    }

    LoopMutator::LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopStatement> loop) : NestDeclarer(nest), _loop(loop) 
    {}

    TileMutator::TileMutator(std::shared_ptr<Nest> nest, std::shared_ptr<TileStatement> tile) : NestDeclarer(nest), _tile(tile) 
    {}

}