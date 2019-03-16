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
    NestDeclaration::NestDeclaration(Variable declaredVariable) : _declaredVariable(declaredVariable)
    {}

    Variable NestDeclaration::GetDeclaredVariable() const 
    { 
        return _declaredVariable; 
    }

    void NestDeclaration::SetPosition(double Position) 
    { 
        _position = Position; 
    }

    double NestDeclaration::GetPosition() const 
    { 
        return _position; 
    }

    LoopDeclaration::LoopDeclaration(Variable indexVariable, int start, int stop, int step) : NestDeclaration(indexVariable), _start(start), _stop(stop), _step(step) 
    {}

    std::ostream& operator<<(std::ostream& stream, const LoopDeclaration& loopDeclaration)
    {
        stream << "for(int " 
            << loopDeclaration.GetDeclaredVariable() 
            << " = " 
            << loopDeclaration.GetStart() 
            << "; " 
            << loopDeclaration.GetDeclaredVariable() 
            << " < " 
            << loopDeclaration.GetStop()
            << "; "
            << loopDeclaration.GetDeclaredVariable()
            << " += "
            << loopDeclaration.GetStep()
            << ")";
        return stream;
    }

    TileDeclaration::TileDeclaration(Variable tileVariable, Variable matrixVariable, Variable topVariable, Variable leftVariable, int height, int width) 
        : NestDeclaration(tileVariable), _matrixVariable(matrixVariable), _topVariable(topVariable), _leftVariable(leftVariable), _height(height), _width(width) 
    {}

    std::ostream& operator<<(std::ostream& stream, const TileDeclaration& tileDeclaration)
    {
        stream << "float* "
            << tileDeclaration.GetDeclaredVariable()
            << " = Tile("
            << tileDeclaration.GetMatrixVariable()
            << ", "
            << tileDeclaration.GetTopVariable()
            << ", "
            << tileDeclaration.GetLeftVariable()
            << ", "
            << tileDeclaration.GetHeight()
            << ", "
            << tileDeclaration.GetWidth()
            <<");";
        return stream;
    }

    void Nest::AddDeclaration(Nest::NestDeclarationPtr nestDeclaration)
    {
        nestDeclaration->SetPosition(Size());
        _declarations.push_back(nestDeclaration);
    }

    int Nest::Size() const 
    { 
        return (int)_declarations.size(); 
    }

    NestDeclaration& Nest::Back() 
    { 
        return *(_declarations.back()); 
    }

    bool Nest::IsDeclared(Variable variable) const
    {
        for(const auto& declaration : _declarations)
        {
            if(declaration->GetDeclaredVariable() == variable)
            {
                return true;
            }
        }
        return false;
    }

    void Nest::Print(std::ostream& stream) const
    {
        IndentedOutputStream indentedStream(stream);

        // sort the declaratins based on the order
        auto copy = GetSortedDeclarationsCopy();
        VerifyDeclarationPositions(copy);
        
        for(const auto& declaration : copy)
        {
            auto loopDeclaration = std::dynamic_pointer_cast<LoopDeclaration>(declaration);
            if(loopDeclaration != nullptr)
            {
                indentedStream << *loopDeclaration;
                indentedStream << " {" << endl;
                indentedStream.IncreaseIndent();
            }

            auto tileDeclaration = std::dynamic_pointer_cast<TileDeclaration>(declaration);
            if(tileDeclaration != nullptr)
            {
                indentedStream << *tileDeclaration << endl;
            }
        }

        std::reverse(copy.begin(), copy.end());
        for(const auto& declaration : copy)
        {
            auto loopDeclaration = std::dynamic_pointer_cast<LoopDeclaration>(declaration);
            if(loopDeclaration != nullptr)
            {
                indentedStream.DecreaseIndent();
                indentedStream << "}" << endl;
            }
        }
    }

    std::vector<Nest::NestDeclarationPtr> Nest::GetSortedDeclarationsCopy() const
    {
        std::vector<NestDeclarationPtr> copy(_declarations);
        std::sort(copy.begin(), copy.end(), [](const NestDeclarationPtr& a, const NestDeclarationPtr& b) { return a->GetPosition() < b->GetPosition();});
        return copy;
    }

    void Nest::VerifyDeclarationPositions(std::vector<Nest::NestDeclarationPtr> sortedDeclarations) const
    {
        std::unordered_set<Variable> definedVariables;
        for(const auto& declaration : sortedDeclarations)
        {
            auto variable = declaration->GetDeclaredVariable();
            if(definedVariables.find(variable) != definedVariables.end())
            {
                throw std::logic_error("variable " + std::to_string(variable.GetID()) + " multiply defined");
            }

            auto tileDeclaration = std::dynamic_pointer_cast<TileDeclaration>(declaration);
            if(tileDeclaration != nullptr)
            {
                if(definedVariables.find(tileDeclaration->GetMatrixVariable()) == definedVariables.end())
                {
                    throw std::logic_error("tile variable " + std::to_string(variable.GetID()) + " requires previous declaration of matrix variable " + std::to_string(tileDeclaration->GetMatrixVariable().GetID()));
                }

                if(definedVariables.find(tileDeclaration->GetTopVariable()) == definedVariables.end())
                {
                    throw std::logic_error("tile variable " + std::to_string(variable.GetID()) + " requires previous declaration of top variable " + std::to_string(tileDeclaration->GetTopVariable().GetID()));
                }

                if(definedVariables.find(tileDeclaration->GetLeftVariable()) == definedVariables.end())
                {
                    throw std::logic_error("tile variable " + std::to_string(variable.GetID()) + " requires previous declaration of left variable " + std::to_string(tileDeclaration->GetLeftVariable().GetID()));
                }
            }

            definedVariables.insert(variable);
        }
    }

    NestDeclarer::NestDeclarer(std::shared_ptr<Nest> nest) : _nest(nest) 
    {}

    NestDeclarer NestDeclarer::Using(Variable matrixVariable)
    {
        auto declaration = std::make_shared<NestDeclaration>(matrixVariable);
        _nest->AddDeclaration(declaration);
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

    LoopMutator::LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopDeclaration> loop) : NestDeclarer(nest), _loop(loop) 
    {}

    TileMutator::TileMutator(std::shared_ptr<Nest> nest, std::shared_ptr<TileDeclaration> tile) : NestDeclarer(nest), _tile(tile) 
    {}

}