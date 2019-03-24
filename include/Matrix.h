////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Matrix.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <vector>

namespace tiler
{
    enum class MatrixOrder { rowMajor, columnMajor };

    class MatrixLayout
    {
    public:
        MatrixLayout(int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize);
        MatrixLayout(int numRows, int numColumns, MatrixOrder order);

        int NumRows() const { return _numRows; }
        int NumColumns() const { return _numColumns; }
        int Size() const { return _numRows * _numColumns; }
        int GetLeadingDimensionSize() const { return _leadingDimensionSize; }
        MatrixOrder GetOrder() const { return _order; }
        int operator()(int row, int column) const;

    private:
        int _numRows;
        int _numColumns;
        MatrixOrder _order; 
        int _leadingDimensionSize;
    };

    // Creates a vector of values from matrix-style initializer lists
    std::vector<float> MatrixToVector(MatrixOrder order, std::initializer_list<std::initializer_list<float>> list);
}