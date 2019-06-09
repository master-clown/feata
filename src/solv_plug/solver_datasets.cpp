#include "solv_plug.hpp"
#include "mesh/meshinfo.hpp"
#include "mathdef.hpp"
#include "tetelem.hpp"


int SolvPlugin::GetNodalDataSet(const int dataset_id, double* nodal_array) const
{
    if(!solv_info_ || !solv_info_->Mesh || !solv_info_->Mesh->NodeLst ||
       !solv_info_->Mesh->NodeNum || !solv_info_->DispLst || !solv_params_)
        return 1;

    if(1 <= dataset_id && dataset_id <= 3) // disp
    {
        for(auto i = 0u; i < solv_info_->Mesh->NodeNum; ++i)
            nodal_array[i] = solv_info_->DispLst[3*i + dataset_id - 1];

        return 0;
    }
    else
    if(dataset_id == 4) // Total U
    {
        for(auto i = 0u; i < solv_info_->Mesh->NodeNum; ++i)
            nodal_array[i] = fabs(solv_info_->DispLst[3*i + 0]) +
                    fabs(solv_info_->DispLst[3*i + 1]) +
                    fabs(solv_info_->DispLst[3*i + 2]);

        return 0;
    }
    else
    if(5 <= dataset_id && dataset_id <= 16) // strain or stress, which require element data
    {
        const auto node_lst = solv_info_->Mesh->NodeLst;
        const auto elem_lst = solv_info_->Mesh->ElemLst;
        const auto disp_lst = solv_info_->DispLst;

        const bool is_stress= dataset_id >= 11;
        const auto comp_idx = dataset_id - (is_stress ? 11 : 5); // xx:0, yy:1, zz:2, etc

        std::string tmp_log_buf;
        TetElem tet_el(tmp_log_buf);
        Mat sfd_mat = Mat::Zero(6, 4*3);                            // element shape func derivatives
        Mat node_mat = Mat::Zero(4, 3);
        const auto mial_mat = TetElem::BuildIsoMatHookeTensor(solv_params_->Young, solv_params_->Poisn);

        std::vector<uint16_t> num_node_shared_elem_lst(solv_info_->Mesh->NodeNum, 0);    // how many elems share a given node

        std::vector<real> elem_strain(6*solv_info_->Mesh->ElemNum, 0.0);                 // exx, eyy, etc
        std::vector<real> elem_stress(is_stress ? solv_info_->Mesh->ElemNum : 0, 0.0);   // only one comp

        for (uint i_el = 0; i_el < solv_info_->Mesh->ElemNum; ++i_el)
        {
            for (int i_node = 0; i_node < 4; ++i_node)
                for(int i_coord = 0; i_coord < 3; ++i_coord)
                    node_mat(i_node, i_coord) = node_lst[3*(elem_lst[4*i_el + i_node] - 1) + i_coord];

            if(!tet_el.Init(i_el, node_mat, solv_params_->Young, solv_params_->Poisn))
                return false;

            tet_el.BuildShapeFuncDerMat(sfd_mat);

            for(int i_comp = 0; i_comp < 6; ++i_comp)
            for(int i_node = 0; i_node < 4; ++i_node)
            {
                elem_strain[6*i_el+i_comp] += sfd_mat(i_comp, 3*i_node+0)*disp_lst[3*(elem_lst[4*i_el + i_node] - 1) + 0] +
                                              sfd_mat(i_comp, 3*i_node+1)*disp_lst[3*(elem_lst[4*i_el + i_node] - 1) + 1] +
                                              sfd_mat(i_comp, 3*i_node+2)*disp_lst[3*(elem_lst[4*i_el + i_node] - 1) + 2];
            }

            if(!is_stress)      // nodal strain
            {
                for(int i_node = 0; i_node < 4; ++i_node)
                {
                    const auto node_idx = elem_lst[4*i_el + i_node] - 1;
                    nodal_array[node_idx] = (nodal_array[node_idx]*num_node_shared_elem_lst[node_idx] +
                                             elem_strain[6*i_el+comp_idx]) / (num_node_shared_elem_lst[node_idx] + 1);
                    ++num_node_shared_elem_lst[node_idx];
                }
            }
            else                // nodal stress
            {
                for(int i_comp = 0; i_comp < 6; ++i_comp)
                    elem_stress[i_el] += mial_mat(comp_idx, i_comp) * elem_strain[6*i_el+i_comp];

                for(int i_node = 0; i_node < 4; ++i_node)
                {
                    const auto node_idx = elem_lst[4*i_el + i_node] - 1;
                    nodal_array[node_idx] = (nodal_array[node_idx]*num_node_shared_elem_lst[node_idx] +
                                             elem_stress[i_el]) / (num_node_shared_elem_lst[node_idx] + 1);
                    ++num_node_shared_elem_lst[node_idx];
                }
            }
        }

        return 0;
    }

    return 2;
}
