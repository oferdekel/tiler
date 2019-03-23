////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Matrix.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix.h"

#include <iomanip>
#include <stdexcept>

namespace  tiler
{
    int InitLeadingDimensionSize(int numRows, int numColumns, MatrixOrder order)
    {
        if(order == MatrixOrder::rowMajor)
        {
            return numColumns;
        }
        else
        {
            return numRows;
        }

    }

    MatrixLayout::MatrixLayout(int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize) : _numRows(numRows), _numColumns(numColumns), _order(order), _leadingDimensionSize(leadingDimensionSize)
    {}

    Matrix::Matrix(float* data, int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize) : MatrixLayout(numRows, numColumns, order, leadingDimensionSize), _data(data) 
    {}

    Matrix::Matrix(float* data, int numRows, int numColumns, MatrixOrder order) : MatrixLayout(numRows, numColumns, order, InitLeadingDimensionSize(numRows, numColumns, order)), _data(data) 
    {}

    float* Matrix::GetPointer(int i, int j) 
    {
        if(GetOrder() == MatrixOrder::rowMajor)
        {
            return _data + i * GetLeadingDimensionSize() + j;
        }
        else
        {
            return _data + i + j * GetLeadingDimensionSize();
        }
    }

    const float* Matrix::GetPointer(int i, int j) const 
    {
        if(GetOrder() == MatrixOrder::rowMajor)
        {
            return _data + i * GetLeadingDimensionSize() + j;
        }
        else
        {
            return _data + i + j * GetLeadingDimensionSize();
        }
    }

    void Matrix::Print(std::ostream& stream) const
    {
        stream << "{ {" << std::setw(5) << setiosflags(std::ios::fixed) << std::setprecision(2) << (*this)(0, 0);
        for(int j=1; j<NumColumns(); ++j)
        {
            stream << ", " << std::setw(5) << setiosflags(std::ios::fixed) << std::setprecision(2) << (*this)(0, j);
        }
        stream << " }";

        for(int i=1; i<NumRows(); ++i)
        {
            stream << ",\n  { " << (*this)(i, 0);
            for(int j=1; j<NumColumns(); ++j)
            {
                stream << ", " << std::setw(5) << setiosflags(std::ios::fixed) << std::setprecision(2) << (*this)(i, j);
            }
            stream << " }";
        }
        stream << " }\n";
    }

    void Matrix::PrintData(std::ostream& stream) const
    {
        stream << "{" << std::setw(5) << setiosflags(std::ios::fixed) << std::setprecision(2) << _data[0];
        for(int i=1; i<NumRows() * NumColumns(); ++i)
        {
            stream << ", " << std::setw(5) << setiosflags(std::ios::fixed) << std::setprecision(2) << _data[i];
        }
        stream << " }";
    }

    std::ostream& operator<<(std::ostream& stream, const Matrix& matrix)
    {
        matrix.Print(stream);
        return stream;
    }

    std::vector<float> MatrixToVector(MatrixOrder order, std::initializer_list<std::initializer_list<float>> list)
    {
        int numRows = (int)list.size();
        int numColumns = (int)(list.begin()->size());

        std::vector<float> v(numRows * numColumns);
        Matrix M(v.data(), numRows, numColumns, order);

        int i = 0;
        for (auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
        {
            // verify initializer list size
            if(rowIter->size() != numColumns)
            {
                throw std::logic_error("incorrect number of elements in matrix initializer list");
            }

            int j = 0;
            for (auto elementIter = rowIter->begin(); elementIter < rowIter->end(); ++elementIter)
            {
                M(i, j) = *elementIter;
                ++j;
            }
            ++i;
        }

        return v;
    }
} 