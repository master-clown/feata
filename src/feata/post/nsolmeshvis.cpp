#include "post/nsolmeshvis.hpp"
#include <MeshVS_Mesh.hxx>
#include <MeshVS_DeformedDataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_NodalColorPrsBuilder.hxx>
#include "defs/common.hpp"
#include "mesh/meshdatasource.hpp"
#include "mesh/meshinfo.hpp"
#include "post/nodalsol.hpp"
#include "solv/solvinfo.hpp"
#include "util/log.hpp"


namespace post
{
    static void GetColorMap(const NodalSol& sol, TColStd_DataMapOfIntegerReal& cmap, double& vmin, double& vmax);

    struct NsolMeshVis::NsolMeshVisPriv
    {
        // if ever rendered, OCC will delete it
        Handle(MeshVS_Mesh)                 Mesh;
        Handle(MeshVS_DeformedDataSource)   MeshDataSrc;
        Handle(MeshVS_NodalColorPrsBuilder) MeshVS;
        double MinResVal = 0.0;
        double MaxResVal = 0.0;

        NsolMeshVisPriv() = default;
       ~NsolMeshVisPriv() = default;
    };

    NsolMeshVis::NsolMeshVis() : priv_(new NsolMeshVisPriv)  {}
    NsolMeshVis::NsolMeshVis(NodalSol* info) : NsolMeshVis() { Init(info); }
    NsolMeshVis::~NsolMeshVis()                              { Clear(); }

    void NsolMeshVis::Init(NodalSol* info)
    {
        if((uint)info->DataSet.size() != info->Sol->Mesh->NodeNum)
        {
            util::logger::PrintE("Nodal solution for rendering is invalid.");
            return;
        }
        if(!priv_) priv_ = new NsolMeshVisPriv();

        sol_ = info;

        priv_->Mesh = new MeshVS_Mesh();
        Handle(mesh::MeshDataSource) mds = new mesh::MeshDataSource(sol_->Sol->Mesh);

        priv_->MeshDataSrc = new MeshVS_DeformedDataSource(mds, 1.0);
        for(auto i = 0u; i < sol_->Sol->Mesh->NodeNum; ++i)
            priv_->MeshDataSrc->SetVector(i + 1, {sol_->Sol->DispLst[3*i + 0],
                                                  sol_->Sol->DispLst[3*i + 1],
                                                  sol_->Sol->DispLst[3*i + 2] });

        priv_->MeshVS = new MeshVS_NodalColorPrsBuilder(priv_->Mesh, MeshVS_DMF_OCCMask | MeshVS_DMF_NodalColorDataPrs,
                                                        0, 1);      // builder has ID 1
        priv_->MeshVS->UseTexture(true);

        Aspect_SequenceOfColor color_map;
        color_map.Append(Quantity_NOC_BLUE1);
        color_map.Append(Quantity_NOC_GREEN1);
        color_map.Append(Quantity_NOC_RED);
        priv_->MeshVS->SetColorMap(color_map);

        TColStd_DataMapOfIntegerReal cmap;
        GetColorMap(*sol_, cmap, priv_->MinResVal, priv_->MaxResVal);

        priv_->MeshVS->SetInvalidColor(Quantity_NOC_BLACK);
        priv_->MeshVS->SetTextureCoords(cmap);

        priv_->Mesh->SetDataSource(priv_->MeshDataSrc);
        priv_->Mesh->AddBuilder(priv_->MeshVS);

        //priv_->Mesh->SetDisplayMode(MeshVS_DMF_Shading | MeshVS_DMF_NodalColorDataPrs);
        priv_->Mesh->GetDrawer()->SetBoolean(MeshVS_DA_DisplayNodes, false);
        priv_->Mesh->GetDrawer()->SetColor(MeshVS_DA_EdgeColor, Quantity_NOC_BLACK);
        //priv_->Mesh->GetDrawer()->SetMaterial(MeshVS_DA_FrontMaterial, Graphic3d_NOM_STEEL);
    }

    void NsolMeshVis::Clear() { SAFE_DEL(priv_); sol_ = nullptr; }

    MeshVS_Mesh* NsolMeshVis::GetOCCMesh() const { return priv_ ? priv_->Mesh.get() : nullptr; }

    double NsolMeshVis::GetMinResValue() const { return priv_ ? priv_->MinResVal : 0.0; }
    double NsolMeshVis::GetMaxResValue() const { return priv_ ? priv_->MaxResVal : 0.0; }

    void GetColorMap(const NodalSol& sol, TColStd_DataMapOfIntegerReal& cmap, double& vmin, double& vmax)
    {
        const auto mima = std::minmax_element(sol.DataSet.cbegin(), sol.DataSet.cend());
        vmin = *mima.first; vmax = *mima.second;
        const auto diff = vmax - vmin;

        for(int i = 0; i < sol.DataSet.size(); ++i)
        {
            cmap.Bind(i+1, (sol.DataSet[i] - vmin) / diff);
        }
    }
}
