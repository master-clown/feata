#pragma once

#include <string>
#include "mathdef.hpp"


/**
 * \brief The TetElem class
 *     Represents tetrahedral element with linear interpolation order.
 */
class TetElem
{
public:
    TetElem(std::string& log_buf);

    bool Init(const uint i_elem,
              const Mat& node_mat,			// 4x3 (i-th col - (i+1)-th node's coordinates)
              const real e_modulus,			// Young's modulus
              const real nu);				// Poisson's ratio

    Mat BuildStifMat() const;

private:
    Mat node_mat_;
    real e_modulus_;
    real nu_;

    Mat A_;
    real V6_;		// tetrahedron volume times 6!
    Vec a_, b_, c_, d_;
    Mat D_;

    std::string& log_buf_;
};
