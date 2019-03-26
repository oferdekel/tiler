////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     MatrixLayout.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <vector>

namespace tiler
{
    enum class MatrixOrder { rowMajor, columnMajor };

    // represents the layout of a matrix (size, order, etc)
    class MatrixLayout
    {
    public:
        // Constructors
        MatrixLayout(int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize);
        MatrixLayout(int numRows, int numColumns, MatrixOrder order);

        // Access layout parameters
        int NumRows() const { return _numRows; }
        int NumColumns() const { return _numColumns; }
        MatrixOrder GetOrder() const { return _order; }
        int GetLeadingDimensionSize() const { return _leadingDimensionSize; }
        int Size() const;
        int GetMajorSize() const;
        int GetMinorSize() const;

        // Calculates the offset of a matrix element
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