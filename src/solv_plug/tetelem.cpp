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

    return (V6_/6.) * B.transpose() * D_ * B;
}
