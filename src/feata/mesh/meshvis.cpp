#include "mesh/meshvis.hpp"
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include "mesh/meshdatasource.hpp"
#include "mesh/meshinfo.hpp"
#include "defs/common.hpp"


namespace mesh
{
    struct MeshVis::MeshVisPriv
    {
        // if ever rendered, OCC will delete it
        Handle(MeshVS_Mesh)           Mesh;
        Handle(MeshDataSource)        MeshDataSrc;
        Handle(MeshVS_MeshPrsBuilder) MeshVS;

        MeshVisPriv() = default;
       ~MeshVisPriv() = default;
    };

    MeshVis::MeshVis() : priv_(new MeshVisPriv)  {}
    MeshVis::MeshVis(MeshInfo* info) : MeshVis() { Init(info); }
    MeshVis::~MeshVis()                          { Clear(); }

    void MeshVis::Init(MeshInfo* info)
    {
        if(!priv_) priv_ = new MeshVisPriv();

        mesh_ = info;

        priv_->Mesh = new MeshVS_Mesh();
        priv_->MeshDataSrc = new MeshDataSource(mesh_);
        priv_->MeshVS = new MeshVS_MeshPrsBuilder(priv_->Mesh, MeshVS_DMF_OCCMask, priv_->MeshDataSrc);

        priv_->Mesh->SetDataSource(priv_->MeshDataSrc);
        priv_->Mesh->AddBuilder(priv_->MeshVS);
        priv_->Mesh->SetDisplayMode(MeshVS_DMF_Shading);
        priv_->Mesh->GetDrawer()->SetBoolean(MeshVS_DA_DisplayNodes, false);
        priv_->Mesh->GetDrawer()->SetColor(MeshVS_DA_EdgeColor, Quantity_NOC_BLACK);
        priv_->Mesh->GetDrawer()->SetMaterial(MeshVS_DA_FrontMaterial, Graphic3d_NOM_STEEL);
    }

    void MeshVis::Clear() { SAFE_DEL(priv_); mesh_ = nullptr; }

    MeshVS_Mesh* MeshVis::GetOCCMesh() const { return priv_ ? priv_->Mesh.get() : nullptr; }
}
