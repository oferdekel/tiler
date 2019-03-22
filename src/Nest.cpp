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
    NestStatementBase::NestStatementBase(Variable declaredVariable) : _declaredVariable(declaredVariable)
    {}

    Variable NestStatementBase::GetDeclaredVariable() const 
    { 
        return _declaredVariable; 
    }

    void NestStatementBase::SetPosition(double Position) 
    { 
        _position = Position; 
    }

    double NestStatementBase::GetPosition() const 
    { 
        return _position; 
    }

    UsingDeclaration::UsingDeclaration(Matrix matrix) : _matrix(matrix)
    {}

    std::ostream& operator<<(std::ostream& stream, const UsingDeclaration& usingStatement)
    {
        auto matrix = usingStatement.GetMatrix();
        stream << "float " 
            << usingStatement.GetMatrix().GetName() 
            << "["
            << matrix.Size()
            << "] = ";
        matrix.PrintData(stream);
        stream << ";";

        return stream;
    }

    LoopStatement::LoopStatement(Variable indexVariable, int start, int stop, int step) : NestStatementBase(indexVariable), _start(start), _stop(stop), _step(step) 
    {}

    std::ostream& operator<<(std::ostream& stream, const LoopStatement& loopStatement)
    {
        stream << "for(int " 
            << loopStatement.GetDeclaredVariable().GetName()
            << " = " 
            << loopStatement.GetStart() 
            << "; " 
            << loopStatement.GetDeclaredVariable().GetName()
            << " < " 
            << loopStatement.GetStop()
            << "; "
            << loopStatement.GetDeclaredVariable().GetName()
            << " += "
            << loopStatement.GetStep()
            << ")";

        return stream;
    }

    TileStatement::TileStatement(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width) 
        : NestStatementBase(tileVariable), _matrixVariable(matrixVariable), _topVariable(topVariable), _leftVariable(leftVariable), _height(height), _width(width) 
    {}

    std::ostream& operator<<(std::ostream& stream, const TileStatement& tileStatement)
    {
        stream << "float* "
            << tileStatement.GetDeclaredVariable().GetName()
            << " = Tile("
            << tileStatement.GetMatrixVariable().GetName()
            << ", "
            << tileStatement.GetTopVariable().GetName()
            << ", "
            << tileStatement.GetLeftVariable().GetName()
            << ", "
            << tileStatement.GetHeight()
            << ", "
            << tileStatement.GetWidth()
            <<");";
        return stream;
    }

    void Nest::AddStatement(Nest::NestStatementPtr nestStatement)
    {
        nestStatement->SetPosition(Size());
        _statements.push_back(nestStatement);
    }

    void Nest::AddUsingDeclaration(Nest::UsingDeclarationPtr usingDeclaration)
    {
        _usingDeclarations.push_back(usingDeclaration);
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

        indentedStream << "// initialize" << endl;
        for(const auto& usingDeclaration : _usingDeclarations)
        {
            indentedStream << *usingDeclaration << endl;
        }

        // sort the declaratins based on the order
        auto copy = GetSortedStatementsCopy();
        VerifyStatementPositions(copy);
        
        indentedStream << endl << "// loop" << endl;
        for(const auto& declaration : copy)
        {
            auto usingStatement = std::dynamic_pointer_cast<UsingDeclaration>(declaration);
            if(usingStatement != nullptr)
            {
                indentedStream << *usingStatement << endl;
            }

            auto loopStatement = std::dynamic_pointer_cast<LoopStatement>(declaration);
            if(loopStatement != nullptr)
            {
                indentedStream << *loopStatement;
                indentedStream << " {" << endl;
                indentedStream.IncreaseIndent();
            }

            auto tileStatement = std::dynamic_pointer_cast<TileStatement>(declaration);
            if(tileStatement != nullptr)
            {
                indentedStream << *tileStatement << endl;
            }
        }

        std::reverse(copy.begin(), copy.end());
        for(const auto& declaration : copy)
        {
            auto loopStatement = std::dynamic_pointer_cast<LoopStatement>(declaration);
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
        std::sort(copy.begin(), copy.end(), [](const NestStatementPtr& a, const NestStatementPtr& b) { return a->GetPosition() < b->GetPosition();});
        return copy;
    }

    void Nest::VerifyStatementPositions(std::vector<Nest::NestStatementPtr> sortedStatements) const
    {
        std::unordered_set<Variable> definedVariables;
        
        for(const auto& usingDeclaration : _usingDeclarations)
        {
            definedVariables.insert(usingDeclaration->GetMatrix());
        }
        
        for(const auto& declaration : sortedStatements)
        {
            auto variable = declaration->GetDeclaredVariable();
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
        auto declaration = std::make_shared<UsingDeclaration>(matrix);
        _nest->AddUsingDeclaration(declaration);
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