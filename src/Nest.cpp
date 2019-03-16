////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     ForAll.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nest.h"
#include "IndentedOutputStream.h"

#include <ctime>
#include <algorithm>

namespace tiler
{
    // initialize the variable counter
    int Variable::_counter = 0;

    Variable::Variable() : _id(_counter++)
    {}

    bool Variable::operator==(const Variable& other)
    {
        return _id == other._id;
    }

    std::ostream& operator<<(std::ostream& stream, const Variable& variable)
    {
        stream << 'v' << variable.GetID();
        return stream;
    }

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

    LoopDeclaration::LoopDeclaration(Variable index, int start, int stop, int step) : NestDeclaration(index), _start(start), _stop(stop), _step(step) 
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

    TileDeclaration::TileDeclaration(Variable tileVariable, Variable matrix, Variable top, Variable left, int height, int width) 
        : NestDeclaration(tileVariable), _matrix(matrix), _top(top), _left(left), _height(height), _width(width) 
    {}

    void Nest::AddDeclaration(Nest::NestDeclarationPtr nestDeclaration)
    {
        if(IsDeclared(nestDeclaration->GetDeclaredVariable()))
        {
            throw std::logic_error("variable defined in previous element");
        }

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
        std::vector<NestDeclarationPtr> copy(_declarations);
        std::sort(copy.begin(), copy.end(), [](const NestDeclarationPtr& a, const NestDeclarationPtr& b) { return a->GetPosition() < b->GetPosition();});

        for(const auto& declaration : copy)
        {
            auto loopDeclaration = std::dynamic_pointer_cast<LoopDeclaration>(declaration);
            if(loopDeclaration != nullptr)
            {
                indentedStream << *loopDeclaration << endl;
                indentedStream << "{" << endl;
                indentedStream.IncreaseIndent();
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

    NestDeclarer::NestDeclarer(std::shared_ptr<Nest> nest) : _nest(nest) 
    {}

    NestDeclarer NestDeclarer::Using(Variable matrix)
    {
        auto declaration = std::make_shared<NestDeclaration>(matrix);
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