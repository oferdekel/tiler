////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Matrix.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace tiler
{
    enum class MatrixOrder { rowMajor, columnMajor };

    class Matrix
    {
    public:
        Matrix(float* data, size_t numRows, size_t numColumns, size_t leadingDimensionSize, MatrixOrder order) {}

        size_t NumRows() const { return _numRows; }
        size_t NumColumns() const { return _numColumns; }
        size_t GetLeadingDimensionSize() const { return _leadingDimensionSize; }
        MatrixOrder GetOrder() const { return _order; }

        float* GetPointerTo(int row, int column) {}
        const float* GetPointerTo(int row, int column) const {}

        

    private:
        float* _data;
        size_t _numRows;
        size_t _numColumns;
        size_t _leadingDimensionSize;
        MatrixOrder _order;
    };
}