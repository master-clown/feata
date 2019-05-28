#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>


using real = double;
using uint = unsigned int;

using Mat = Eigen::MatrixXd;
using Vec = Eigen::VectorXd;
using Vec3 = Eigen::Vector3d;
using Vec3i = Eigen::Vector3i;
using Vec4i = Eigen::Vector4i;
using SpMat = Eigen::SparseMatrix<double, Eigen::ColMajor>;
using SpVec = Eigen::SparseVector<double, Eigen::ColMajor>;
