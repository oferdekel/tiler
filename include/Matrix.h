////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     Matrix.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"

#include <iostream>
#include <vector>

namespace tiler
{
    enum class MatrixOrder { rowMajor, columnMajor };

    class Matrix : public Variable
    {
    public:
        // Constructs a matrix
        Matrix(float* data, int numRows, int numColumns, MatrixOrder order, int leadingDimensionSize);
        Matrix(float* data, int numRows, int numColumns, MatrixOrder order);

        // Access matrix dimensions and info
        int NumRows() const { return _numRows; }
        int NumColumns() const { return _numColumns; }
        int Size() const { return _numRows * _numColumns; }
        int GetLeadingDimensionSize() const { return _leadingDimensionSize; }
        MatrixOrder GetOrder() const { return _order; }

        // Access matrix elements and their memory positions
        float& operator()(int i, int j) { return *GetPointer(i, j); }
        float operator()(int i, int j) const { return *GetPointer(i, j); }
        float* GetPointer(int i, int j);
        const float* GetPointer(int i, int j) const;

        // Print a matrix to an output stream
        void Print(std::ostream& stream) const;
        
        // Print the underlying data as a vector
        void PrintData(std::ostream& stream) const;

    private:
        float* _data;
        int _numRows;
        int _numColumns;
        int _leadingDimensionSize;
        MatrixOrder _order;
    };

    // Print a matrix to an output stream
    std::ostream& operator<<(std::ostream& stream, const Matrix& matrix);

    // Creates a vector of values from matrix-style initializer lists
    std::vector<float> MatrixToVector(MatrixOrder order, std::initializer_list<std::initializer_list<float>> list);

}