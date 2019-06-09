#include "tetelem.hpp"
#include <Eigen/LU>

#define ZERO_DET_EPS 1e-14


TetElem::TetElem(std::string& log_buf)
    : log_buf_(log_buf)
{}

bool TetElem::Init(const uint i_elem,
                   const Mat &node_mat,
                   const real e_modulus,
                   const real nu)
{
    node_mat_ = node_mat;
    e_modulus_ = e_modulus;
    nu_ = nu;

    A_ = Mat::Zero(4, 4);
    A_ << Vec::Ones(4, 1), node_mat_.block(0, 0, 4, 3);
    V6_ = A_.determinant();

    if (abs(V6_) == 0.0)
    {
        log_buf_ = "Zero determinant for the " + std::to_string(i_elem) + "th element.";
        return false;
    }

    const Mat tmp = V6_ * A_.inverse().transpose();
    a_ = tmp.col(0);
    b_ = tmp.col(1);
    c_ = tmp.col(2);
    d_ = tmp.col(3);

    D_ = Mat::Zero(6, 6);
    D_(0, 0) = D_(1, 1) = D_(2, 2) = e_modulus*(1.0 - nu) / ((1.0 + nu) * (1.0 - 2*nu));
    D_(0, 1) = D_(0, 2) = D_(1, 2) =
    D_(1, 0) = D_(2, 0) = D_(2, 1) = e_modulus*nu / ((1.0 + nu) * (1.0 - 2*nu));
    D_(3, 3) = D_(4, 4) = D_(5, 5) = e_modulus / (2.0 + 2.0*nu);

    return true;
}

Mat TetElem::BuildStifMat() const
{
    Mat B = Mat::Zero(6, 3*4);
    BuildShapeFuncDerMat(B);

    return (V6_/6.) * B.transpose() * D_ * B;
}

void TetElem::BuildShapeFuncDerMat(Mat& B) const
{
    for(int i = 0; i < 4; ++i)
    {
        B(0, 0 + 3*i) = b_[i];
        B(1, 1 + 3*i) = c_[i];
        B(2, 2 + 3*i) = d_[i];
        B(3, 0 + 3*i) = c_[i];
        B(3, 1 + 3*i) = b_[i];
        B(4, 1 + 3*i) = d_[i];
        B(4, 2 + 3*i) = c_[i];
        B(5, 0 + 3*i) = d_[i];
        B(5, 2 + 3*i) = b_[i];
    }

    B /= V6_;
}

void TetElem::BuildShapeFuncDerRow(Vec& row, const int idx) const
{
    switch(idx)
    {
    case 0: for(int i=0;i<4;++i) row(0 + 3*i) = b_[i]; break;
    case 1: for(int i=0;i<4;++i) row(1 + 3*i) = c_[i]; break;
    case 2: for(int i=0;i<4;++i) row(2 + 3*i) = d_[i]; break;
    case 3: for(int i=0;i<4;++i){row(0 + 3*i) = c_[i]; row(1 + 3*i) = b_[i];} break;
    case 4: for(int i=0;i<4;++i){row(1 + 3*i) = d_[i]; row(2 + 3*i) = c_[i];} break;
    case 5: for(int i=0;i<4;++i){row(0 + 3*i) = d_[i]; row(2 + 3*i) = b_[i];} break;
    default: return;
    }

    row /= V6_;
}

Mat TetElem::BuildIsoMatHookeTensor(const real e_modulus,
                                    const real nu)
{
    Mat m = Mat::Zero(6, 6);
    m(0, 0) = m(1, 1) = m(2, 2) = 1 - nu;
    m(3, 3) = m(4, 4) = m(5, 5) = 0.5 - nu;
    m(0, 1)=m(1, 0) = m(0, 2)=m(2, 0) = m(1, 2)=m(1, 2) = nu;

    return e_modulus / ((1 + nu)*(1 - 2*nu)) * m;
}
