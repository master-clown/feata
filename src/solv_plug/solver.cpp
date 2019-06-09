#include "solv_plug.hpp"
#include <Eigen/SparseCholesky>
#include <Eigen/Geometry>
#include <set>
#include "mesh/meshinfo.hpp"
#include "tetelem.hpp"


struct Triplet      // for Eigen interface
{
    int Row;
    int Col;
    real Value;

    int row() const { return Row; }
    int col() const { return Col; }
    real value() const { return Value; }
};

static void ApplyBc(const MeshInfo* mesh,
                    const SolvParams& params,
                    SpMat& mat,
                    SpVec& rhs);
static std::string SolvStatusToStr(const Eigen::ComputationInfo st);

bool SolvPlugin::Solve_(std::string& log_buf)
{
    const auto node_lst = solv_info_->Mesh->NodeLst;
    const auto elem_lst = solv_info_->Mesh->ElemLst;
    const auto node_num = solv_info_->Mesh->NodeNum;
    const auto elem_num = solv_info_->Mesh->ElemNum;

    const auto dof_count = 3 * node_num;

    TetElem tet_el(log_buf);
    Mat K_loc;
    Mat node_mat = Mat::Zero(4, 3);

    const size_t K_lst_size = elem_num * 4 * 4 * 3 * 3;

    std::vector<Triplet> K_lst;
    try
    {
        K_lst.resize(K_lst_size);
    }
    catch (const std::bad_alloc&)
    {
        log_buf = "Cannot allocate memory for stiffness matrix (" +
                  std::to_string(K_lst_size * sizeof(Triplet)) + " bytes required).";
        return false;
    }

    for (uint i_el = 0, i_k_el = 0; i_el < elem_num; ++i_el)
    {
        // build local stiffness matrices

        for (int i_node = 0; i_node < 4; ++i_node)
            for(int i_coord = 0; i_coord < 3; ++i_coord)
                node_mat(i_node, i_coord) = node_lst[3*(elem_lst[4*i_el + i_node] - 1) + i_coord];

        if(!tet_el.Init(i_el, node_mat, solv_params_->Young, solv_params_->Poisn))
            return false;

        K_loc = tet_el.BuildStifMat();
        for (int i_node = 0; i_node < 4; ++i_node)
        {
            const int z1 = 3 * (elem_lst[4*i_el + i_node] - 1);
            for (int j_node = 0; j_node < 4; ++j_node)
            {
                const int z2 = 3 * (elem_lst[4*i_el + j_node] - 1);
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                    {
                        K_lst[i_k_el].Row = z1 + i;
                        K_lst[i_k_el].Col = z2 + j;
                        K_lst[i_k_el].Value = K_loc(i_node*3 + i, j_node*3 + j);
                        ++i_k_el;
                    }
            }
        }
    }

    SpMat K(dof_count, dof_count);
    K.setFromTriplets(K_lst.cbegin(), K_lst.cend());
    K_lst.clear();

    stats_buf += "Number of DOF: " + std::to_string(dof_count) + '\n';

    SpVec F(dof_count);

    ApplyBc(solv_info_->Mesh, *solv_params_, K, F);

    Eigen::SimplicialLDLT<SpMat> solver;

    solver.compute(K);
    if(auto st = solver.info(); st != Eigen::ComputationInfo::Success)
    {
        log_buf += "Could not decompose stiffness matrix: " + SolvStatusToStr(st);
        return false;
    }

    Vec sol = solver.solve(F);
    if(auto st = solver.info(); st != Eigen::ComputationInfo::Success)
    {
        log_buf += "Could not solve linear system: " + SolvStatusToStr(st);
        return false;
    }

    const auto siz = sol.size();
    for(int i = 0; i < siz; ++i)
        solv_info_->DispLst[i] = sol(i);

    return true;
}

void ApplyBc(const MeshInfo* mesh,
             const SolvParams& params,
             SpMat& mat,
             SpVec& rhs)
{
    const real big_num = 1.0e+28;
    std::set<MeshInfo::id_t> used_node_lst;

    for(auto i_face = 0u; i_face < mesh->ElemFaceNum; ++i_face)
    {
        for(const auto& bc: params.BcLst)
        {
            if(bc.first != mesh->ElemFaceMarkerLst[i_face])
                continue;

            if(bc.second.BcLst.size() == 1 && bc.second.BcLst.front().Tag[0] == 'p')
            {
                const MeshInfo::id_t nodes[] = { mesh->ElemFaceLst[3*i_face + 0] - 1,
                                                 mesh->ElemFaceLst[3*i_face + 1] - 1,
                                                 mesh->ElemFaceLst[3*i_face + 2] - 1 };
                const Vec3 n1_vec(mesh->NodeLst[3*nodes[0]+0], mesh->NodeLst[3*nodes[0]+1], mesh->NodeLst[3*nodes[0]+2]);
                const Vec3 n2_vec(mesh->NodeLst[3*nodes[1]+0], mesh->NodeLst[3*nodes[1]+1], mesh->NodeLst[3*nodes[1]+2]);
                const Vec3 n3_vec(mesh->NodeLst[3*nodes[2]+0], mesh->NodeLst[3*nodes[2]+1], mesh->NodeLst[3*nodes[2]+2]);
                const Vec3 surf_v = (n2_vec - n1_vec).cross(n3_vec - n1_vec);
                const Vec3 pr_vec = (bc.second.BcLst.front().Value/6) * surf_v;

                for(int i = 0; i < 3; ++i)
                    for(int j = 0; j < 3; ++j)
                    {
                        rhs.coeffRef(3*nodes[i]+j) += pr_vec(j);
                        //used_node_lst.insert(nodes[i]+j);  - do we have to do this???
                    }
            }
            else
            {
                for(int i_n = 0; i_n < 3; ++i_n)
                {
                    const auto node = mesh->ElemFaceLst[3*i_face + i_n] - 1;
                    for(const auto& b: bc.second.BcLst)
                    {
                        for(int i_ax = 0; i_ax < 3; ++i_ax)
                            if(b.Tag[i_ax+1] && !used_node_lst.count(3*node+i_ax))
                            {
                                if(b.Tag[0] == 'u')
                                    mat.coeffRef(3*node+i_ax, 3*node+i_ax) = big_num;
                                rhs.coeffRef(3*node+i_ax) = (b.Tag[0] == 'u' ? big_num : 1.0) * b.Value;
                                used_node_lst.insert(3*node+i_ax);
                            }
                    }
                }
            }

            break;
        }
    }
}

std::string SolvStatusToStr(const Eigen::ComputationInfo st)
{
    switch (st)
    {
    case Eigen::ComputationInfo::Success:       return "success";
    case Eigen::ComputationInfo::InvalidInput:  return "the inputs are invalid, or the algorithm has been improperly called";
    case Eigen::ComputationInfo::NoConvergence: return "iterative procedure did not converge";
    case Eigen::ComputationInfo::NumericalIssue:return "the provided data did not satisfy the prerequisites";
    }

    return "";
}
