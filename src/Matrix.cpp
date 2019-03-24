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
    MatrixLayout::MatrixLayout(int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize) : _numRows(numRows), _numColumns(numColumns), _order(order), _leadingDimensionSize(leadingDimensionSize)
    {}

    MatrixLayout::MatrixLayout(int numRows, int numColumns, MatrixOrder order) : _numRows(numRows), _numColumns(numColumns), _order(order), _leadingDimensionSize((order == MatrixOrder::rowMajor) ? numColumns : numRows)
    {}

    std::vector<float> MatrixToVector(MatrixOrder order, std::initializer_list<std::initializer_list<float>> list)
    {
        int numRows = (int)list.size();
        int numColumns = (int)(list.begin()->size());
        int rowSkip = (order == MatrixOrder::rowMajor) ? numColumns : 1;
        int columnSkip = (order == MatrixOrder::columnMajor) ? numRows : 1;

        std::vector<float> v(numRows * numColumns);

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
                v[i * rowSkip + j * columnSkip] = *elementIter;
                ++j;
            }
            ++i;
        }

        return v;
    }
} 