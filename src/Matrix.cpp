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
    Matrix::Matrix(float* data, int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize) : _data(data), _numRows(numRows), _numColumns(numColumns), _order(order), _leadingDimensionSize(leadingDimensionSize)
    {}

    Matrix::Matrix(float* data, int numRows, int numColumns, MatrixOrder order) : _data(data), _numRows(numRows), _numColumns(numColumns), _order(order)
    {
        if(_order == MatrixOrder::rowMajor)
        {
            _leadingDimensionSize = _numColumns;
        }
        else
        {
            _leadingDimensionSize = _numRows;
        }
    }

    float* Matrix::GetPointer(int i, int j) 
    {
        if(_order == MatrixOrder::rowMajor)
        {
            return _data + i * _leadingDimensionSize + j;
        }
        else
        {
            return _data + i + j * _leadingDimensionSize;
        }
    }

    const float* Matrix::GetPointer(int i, int j) const 
    {
        if(_order == MatrixOrder::rowMajor)
        {
            return _data + i * _leadingDimensionSize + j;
        }
        else
        {
            return _data + i + j * _leadingDimensionSize;
        }
    }

    void Matrix::Print(std::ostream& stream) const
    {
        stream << "{ {" << std::setw(5) << setiosflags(std::ios::fixed) << std::setprecision(2) << (*this)(0, 0);
        for(int j=1; j<_numColumns; ++j)
        {
            stream << ", " << std::setw(5) << setiosflags(std::ios::fixed) << std::setprecision(2) << (*this)(0, j);
        }
        stream << " }";

        for(int i=1; i<_numRows; ++i)
        {
            stream << ",\n  { " << (*this)(i, 0);
            for(int j=1; j<_numColumns; ++j)
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
        for(int i=1; i<_numRows * _numColumns; ++i)
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