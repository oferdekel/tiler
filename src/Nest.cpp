////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Nest.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nest.h"
#include "PrintUtils.h"

#include <algorithm>

namespace tiler
{
    const char* copyFunction = R"AW(
    void Copy(float* target, const float* source, int size, int count, int targetSkip, int sourceSkip)
    {
        for(int i=0; i<count; ++i)
        {
            std::copy_n(source, size, target);
            source += sourceSkip;
            target += targetSkip;
        }
    }
    )AW";

    const char* copyTransposeFunction = R"AW(
    void CopyTranspose(float* target, const float* source, int size, int count, int targetSkip, int sourceSkip)
    {
        for(int i=0; i<count; ++i)
        {
            for(int j=0; j<size; ++j)
            {
                target[i * sourceSkip + j] = source[i + j * targetSkip];
            }
        }
    }
    )AW";

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
        IncreaseIndent();

        // pre-sort pass 1 - identify required functions
        bool requiresCopy = false;
        bool requiresCopyTranspose = false;
        for(const auto& statement : _statements)
        {
            auto tileStatement = std::dynamic_pointer_cast<TileStatement>(statement);
            if(tileStatement != nullptr)
            {
                if(tileStatement->IsCached())
                {
                    if(tileStatement->IsTransposed())
                    {
                        requiresCopyTranspose = true;
                    }
                    else
                    {
                        requiresCopy = true;
                    }
                }
            }
        }

        if(requiresCopy)
        {
            stream << copyFunction << std::endl;
        }
        if(requiresCopyTranspose)
        {
            stream << copyTransposeFunction << std::endl;
        }

        // start main function
        stream << Indent << "int main()\n" << Indent << "{\n";
        IncreaseIndent();

        // pre-sort pass 2 - set positions of tile statement
        for(const auto& statement : _statements)
        {
            auto tileStatement = std::dynamic_pointer_cast<TileStatement>(statement);
            if(tileStatement != nullptr)
            {
                tileStatement->SetPositionByDependencies();
            }
        }

        // sort the statements by position
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
                if(IsPointerTo<ForAllStatement>(a) && IsPointerTo<TileStatement>(b))
                {
                    return true;
                }

                if(IsPointerTo<ForAllStatement>(b) && IsPointerTo<TileStatement>(a))
                {
                    return false;
                }
            }

            return a->GetPosition() < b->GetPosition();
        };
        std::sort(_statements.begin(), _statements.end(), comparer);

        // post-sort forward pass
        for(const auto& statement : _statements)
        {
            statement->PrintForward(stream);
        }

        // backwards pass
        std::reverse(_statements.begin(), _statements.end());
        for(const auto& statement : _statements)
        {
            statement->PrintBackward(stream);
        }

        //print prefix
        DecreaseIndent();
        stream << Indent << "}";
    }

    NestStatementAppender::NestStatementAppender(std::shared_ptr<Nest> nest) : _nest(nest) 
    {}

    NestStatementAppender NestStatementAppender::Using(Variable matrixVariable, MatrixLayout matrixLayout, bool isOutput, float* data)
    {
        auto statement = std::make_shared<UsingStatement>(matrixVariable, matrixLayout, isOutput, data);
        _nest->AddStatement(statement);
        return *this;
    }

    NestStatementAppender NestStatementAppender::Kernel(const Variable& matrixAVariable, const Variable& matrixBVariable, const Variable& matrixCVariable, KernelStatement::KernelType kernel)
    {
        auto matrixAStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixAVariable);
        auto matrixBStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixBVariable);
        auto matrixCStatement = _nest->FindStatementByTypeAndVariable<MatrixStatement>(matrixCVariable);

        auto kernelStatement = std::make_shared<KernelStatement>(matrixAStatement, matrixBStatement, matrixCStatement, kernel);
        _nest->AddStatement(kernelStatement);
        return *this; 
    }

    void NestStatementAppender::Print(std::ostream& stream) const
    { 
        return _nest->Print(stream); 
    }

    double ForAllStatementModifier::_loopCounter = 0;

    ForAllStatementModifier::ForAllStatementModifier(std::shared_ptr<Nest> nest, std::shared_ptr<ForAllStatement> loop) : NestStatementAppender(nest), _loop(loop) 
    {
        _loop->SetPosition(_loopCounter++);
    }

    ForAllStatementModifier ForAllStatementModifier::Position(double Position) 
    { 
        _loop->SetPosition(Position); 
        return *this; 
    }

    TileStatementModifier::TileStatementModifier(std::shared_ptr<Nest> nest, std::shared_ptr<TileStatement> tile) : NestStatementAppender(nest), _tile(tile) 
    {}

    NestStatementAppender TileStatementModifier::Cache(MatrixOrder order)
    {
        _tile->SetCache(true);
        MatrixLayout oldLayout = _tile->GetLayout();
        MatrixLayout newLayout {oldLayout.NumRows(), oldLayout.NumColumns(), order};
        _tile->GetLayout() = newLayout;

        // add cache allocation
        auto statement = std::make_shared<UsingStatement>(_tile->GetVariable(), newLayout, false, nullptr);
        _nest->AddStatement(statement);

        return NestStatementAppender(_nest);
    }

    NestStatementAppender MakeNest()
    {
        auto nest = std::make_shared<Nest>();
        return NestStatementAppender(nest);
    }
}