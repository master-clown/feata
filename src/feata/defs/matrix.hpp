#pragma once

#include "defs/vec.hpp"


enum MatrixSpecialType
{
    MATRIX_SPECIAL_TYPE_IDENTITY,
    MATRIX_SPECIAL_TYPE_UDIAG,
    MATRIX_SPECIAL_TYPE_LDIAG
};

template<int rowc, int colc, class NumT = coord_t>
class Matrix
{
public:
    Matrix() { static_assert (rowc >= 0 && colc >= 0, "Zero dimensionality forbidden"); }
    
    Matrix(const NumT def_val)
        : Matrix()
    {
        for(auto& el: data_)
            el = def_val;
    }
    Matrix(Vec<rowc, NumT> col_lst[colc])
        : Matrix()
    {
        for(uint i = 0; i < rowc; ++i)
            for(uint j = 0; j < colc; ++j)
                data_[i*colc + j] = col_lst[i][j];
    }
    
    NumT& el(const int i, const int j) { return data_[i*colc + j]; }
    const NumT& el(const int i, const int j) const { return data_[i*colc + j]; }
    
    Matrix operator+(const Matrix& rhs) const
    {
        Matrix res(NumT{});
        
        for(size_t i = 0; i < rowc*colc; ++i)
            res.data_[i] = data_[i] + rhs.data_[i];
        
        return res;
    }
    
    Matrix operator*(const NumT& k) const
    {
        Matrix res(NumT{});
        
        for(size_t i = 0; i < rowc*colc; ++i)
            res.data_[i] = k*data_[i];
        
        return res;
    }
    
    template<class...TArgs>
    static Matrix CreateSpecialMatrix(const MatrixSpecialType type, const NumT param = {1.0}, const TArgs&...args)
    {
        Matrix m(NumT{});
        constexpr auto min_size = std::min(rowc, colc);
        
        if(type == MATRIX_SPECIAL_TYPE_IDENTITY)
            for(int i = 0; i < min_size; ++i)
                m.el(i, i) = param;
        if(type == MATRIX_SPECIAL_TYPE_UDIAG)
            for(int i = 0; i < rowc && i+1 < colc; ++i)
                m.el(i, i+1) = param;
        if(type == MATRIX_SPECIAL_TYPE_LDIAG)
            for(int i = 0; i < rowc - 1 && i < colc; ++i)
                m.el(i+1, i) = param;
        
        return m + CreateSpecialMatrix(args...);
    }
    
private:
    static Matrix CreateSpecialMatrix() { return Matrix(0.0); }
    
    NumT data_[rowc*colc];  // column-major
};
