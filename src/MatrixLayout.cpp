////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     MatrixLayout.cpp
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixLayout.h"

#include <iomanip>
#include <stdexcept>

namespace  tiler
{
    MatrixLayout::MatrixLayout(int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize) : _numRows(numRows), _numColumns(numColumns), _order(order), _leadingDimensionSize(leadingDimensionSize)
    {}

    MatrixLayout::MatrixLayout(int numRows, int numColumns, MatrixOrder order) : _numRows(numRows), _numColumns(numColumns), _order(order), _leadingDimensionSize((order == MatrixOrder::rowMajor) ? numColumns : numRows)
    {}

    int MatrixLayout::operator()(int row, int column) const
    {
        if(_order == MatrixOrder::rowMajor)
        {
            return row * _leadingDimensionSize + column;
        }
        else
        {
            return row + column * _leadingDimensionSize;
        }
    }

    std::vector<float> MatrixToVector(MatrixOrder order, std::initializer_list<std::initializer_list<float>> list)
    {
        int numRows = (int)list.size();
        int numColumns = (int)(list.begin()->size());

        std::vector<float> v(numRows * numColumns);
        MatrixLayout matrixLayout(numRows, numColumns, order);

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
                v[matrixLayout(i,j)] = *elementIter;
                ++j;
            }
            ++i;
        }

        return v;
    }
} 