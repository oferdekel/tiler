////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Matrix.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix.h"

#include <iomanip>

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

    std::ostream& operator<<(std::ostream& stream, const Matrix& matrix)
    {
        matrix.Print(stream);
        return stream;
    }

} 