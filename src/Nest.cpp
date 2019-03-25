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

namespace tiler
{
    const char* copyFunction = R"xxx(
void copy(const float* source, float* target, int size, int count, int sourceSkip, int targetSkip)
{
    for(int i=0; i<count; ++i)
    {
        std::copy_n(source, size, target);
        source += sourceSkip;
        target += targetSkip;
    }
}
)xxx";

    template<typename IsType, typename OrigType>
    bool IsPointerTo(const OrigType& pointer)
    {
        return (std::dynamic_pointer_cast<IsType>(pointer) != nullptr);
    }

    void Nest::AddStatement(Nest::StatementPtr nestStatement)
    {
        _statements.push_back(nestStatement);
    }

    int Nest::Size() const 
    { 
        return (int)_statements.size(); 
    }

    void Nest::Print(std::ostream& stream)
    {

        //print prefix
        stream << copyFunction << endil;
        stream << "int main()" 
            << endil
            << "{";
        IncreaseIndent();

        // pre-sort pass
        for(const auto& statement : _statements)
        {
            auto tileStatement = std::dynamic_pointer_cast<TileStatement>(statement);
            if(tileStatement != nullptr)
            {
                tileStatement->SetPositionByDependencies();

                if(tileStatement->IsCached())
                {
                    auto layout = tileStatement->GetLayout();
                    stream << endil 
                        << "float "
                        << tileStatement->GetVariable().GetName()
                        << "["
                        << layout.Size()
                        << "];    // Allocate cache, rows:"
                        << layout.NumRows()
                        << ", cols:"
                        << layout.NumColumns()
                        << ", order:"
                        << ((layout.GetOrder() == MatrixOrder::rowMajor) ? "row" : "column")
                        << endil;
                }
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
            stream << endil;
            stream << *statement;

            if(IsPointerTo<LoopStatement>(statement))
            {
                stream << endil;
                stream << "{";
                IncreaseIndent();
            }
        }

        // backwards pass
        std::reverse(_statements.begin(), _statements.end());
        for(const auto& statement : _statements)
        {
            if(IsPointerTo<LoopStatement>(statement))
            {
                DecreaseIndent();
                stream << endil << "}";
            }
        }

        //print prefix
        DecreaseIndent();
        stream << endil << "}";
    }

    NestMutatorBase::NestMutatorBase(std::shared_ptr<Nest> nest) : _nest(nest) 
    {}

    NestMutatorBase NestMutatorBase::Using(Variable matrixVariable, MatrixLayout matrixLayout, float* data)
    {
        auto statement = std::make_shared<UsingStatement>(matrixVariable, matrixLayout, data);
        _nest->AddStatement(statement);
        return *this;
    }

    NestMutatorBase NestMutatorBase::Kernel(const Variable& matrixAVariable, const Variable& matrixBVariable, const Variable& matrixCVariable, KernelStatement::KernelType kernel)
    {
        auto matrixAStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixAVariable);
        auto matrixBStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixBVariable);
        auto matrixCStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixCVariable);

        auto kernelStatement = std::make_shared<KernelStatement>(matrixAStatement, matrixBStatement, matrixCStatement, kernel);
        _nest->AddStatement(kernelStatement);
        return *this; 
    }

    void NestMutatorBase::Print(std::ostream& stream) const
    { 
        return _nest->Print(stream); 
    }

    double LoopMutator::_loopCounter = 0;

    LoopMutator::LoopMutator(std::shared_ptr<Nest> nest, std::shared_ptr<LoopStatement> loop) : NestMutatorBase(nest), _loop(loop) 
    {
        _loop->SetPosition(_loopCounter++);
    }

    LoopMutator LoopMutator::Position(double Position) 
    { 
        _loop->SetPosition(Position); 
        return *this; 
    }

    TileMutator::TileMutator(std::shared_ptr<Nest> nest, std::shared_ptr<TileStatement> tile) : NestMutatorBase(nest), _tile(tile) 
    {}

    TileMutator TileMutator::Cache(MatrixOrder order)
    {
        _tile->SetCache(true);
        auto layout = _tile->GetLayout();
        _tile->GetLayout() = MatrixLayout{layout.NumRows(), layout.NumColumns(), order};
        return *this;
    }

    NestMutatorBase MakeNest()
    {
        auto nest = std::make_shared<Nest>();
        return NestMutatorBase(nest);
    }
}