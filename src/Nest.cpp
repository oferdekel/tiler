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

namespace tiler
{
    // initialize the random number generator for Variables
    std::default_random_engine Variable::_generator((int)std::time(0));

    Variable::Variable()
    {
        std::uniform_int_distribution<long> uniform;
        _uid = uniform(_generator);
    }

    bool Variable::operator==(const Variable& other)
    {
        return _uid == other._uid;
    }

    NestElement::NestElement(Variable variable, double order) : _variable(variable), _order(order) 
    {}

    Variable NestElement::GetVariable() const 
    { 
        return _variable; 
    }

    void NestElement::SetOrder(double order) 
    { 
        _order = order; 
    }

    double NestElement::GetOrder() const 
    { 
        return _order; 
    }

    LoopElement::LoopElement(Variable index, int start, int stop, int step, double order) : NestElement(index, order), _start(start), _stop(stop), _step(step) 
    {}

    void Nest::AddElement(std::shared_ptr<NestElement> nestElement)
    {
        if(IsDefined(nestElement->GetVariable()))
        {
            throw std::logic_error("variable defined in previous element");
        }

        _elements.push_back(nestElement);
    }

    int Nest::Size() const 
    { 
        return (int)_elements.size(); 
    }

    NestElement& Nest::Back() 
    { 
        return *(_elements.back()); 
    }

    bool Nest::IsDefined(Variable variable) const
    {
        for(const auto& element : _elements)
        {
            if(element->GetVariable() == variable)
            {
                return true;
            }
        }
        return false;
    }

    NestMutator::NestMutator(std::shared_ptr<Nest> nest) : _nest(nest) 
    {}

    NestMutator NestMutator::Order(double order) 
    { 
        _nest->Back().SetOrder(order); return NestMutator(_nest); 
    }

    int NestMutator::NestSize() const 
    { 
        return _nest->Size(); 
    }

    LoopMutator::LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopElement> loop) : NestMutator(nest), _loop(loop) 
    {}
}