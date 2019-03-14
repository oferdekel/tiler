////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Matrix.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>

namespace tiler
{
    enum class MatrixOrder { rowMajor, columnMajor };

    class Matrix
    {
    public:
        Matrix(float* data, size_t numRows, size_t numColumns, size_t leadingDimensionSize, MatrixOrder order);

        size_t NumRows() const { return _numRows; }
        size_t NumColumns() const { return _numColumns; }
        size_t GetLeadingDimensionSize() const { return _leadingDimensionSize; }
        MatrixOrder GetOrder() const { return _order; }

        float* GetPointer(int i, int j);
        const float* GetPointer(int i, int j) const;

        float& operator()(int i, int j) { return *GetPointer(i, j); }
        float operator()(int i, int j) const { return *GetPointer(i, j); }

        void Print(std::ostream& stream) const;

    private:
        float* _data;
        size_t _numRows;
        size_t _numColumns;
        size_t _leadingDimensionSize;
        MatrixOrder _order;
    };

    std::ostream& operator<<(std::ostream& stream, const Matrix& matrix);
}