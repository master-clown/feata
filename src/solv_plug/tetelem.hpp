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

    /**
     * \brief BuildShapeFuncDerMat
     *     Constructs a matrix of shape functions' derivatives (constant for linear case).
     *     Used for building stiffness matrix and getting stresses and strains from displacement.
     *
     ******WARNING: input matrix 'B' must be already allocated to be 6x12 (3 cols per node,
     *     1 row per DOF component: xx, yy, zz, xy, yz, zx).
     */
    void BuildShapeFuncDerMat(Mat& B) const;

    // if the whole matrix is not required
    void BuildShapeFuncDerRow(Vec& row, const int idx) const;

    /**
     * \brief BuildIsoMatHookeTensor
     *     Constructs a symmetric matrix, which links stress and strain by the Hooke's law
     *     for isotropic material.
     */
    static Mat BuildIsoMatHookeTensor(const real e_modulus, const real nu);

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
