#include "solv_plug.hpp"
#include "mesh/meshinfo.hpp"
#include "mathdef.hpp"


int SolvPlugin::GetNodalDataSet(const int dataset_id, double* nodal_array) const
{
    if(!solv_info_ || !solv_info_->Mesh || !solv_info_->Mesh->NodeLst ||
       !solv_info_->Mesh->NodeNum || !solv_info_->DispLst)
        return 1;

    if(dataset_id == 1) // Sxx
    {
        for(auto i = 0u; i < solv_info_->Mesh->NodeNum; ++i)
            nodal_array[i] = fabs(solv_info_->DispLst[3*i + 0]) +
                    fabs(solv_info_->DispLst[3*i + 1]) +
                    fabs(solv_info_->DispLst[3*i + 2]);

        return 0;
    }
    if(dataset_id == 2) // Ux
    {
        for(auto i = 0u; i < solv_info_->Mesh->NodeNum; ++i)
            nodal_array[i] = solv_info_->DispLst[3*i + 0];

        return 0;
    }


    return 2;
}
