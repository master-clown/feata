#include "gui/renderlist.hpp"
#include <Standard_Handle.hxx>
#include <AIS_Shape.hxx>   //?
#include <MeshVS_Mesh.hxx> //?
#include "defs/common.hpp"
#include "geom/geomvis.hpp"
#include "mesh/meshvis.hpp"
#include "post/nsolmeshvis.hpp"
#include "stl/map.hpp"


class AIS_Shape;
class MeshVS_Mesh;

namespace gui
{
    struct NsolPair
    {
        post::NsolMeshVis* NSol;
        Handle(MeshVS_Mesh) MeshVs;
    };

    struct RenderList::RenderListPriv
    {
        geom::GeomVis* CurrGeometry = nullptr;
        geom::GeomVis* CurrTrianGeometry = nullptr;
        mesh::MeshVis* CurrMesh = nullptr;
        std::pair<int, int> CurrNsol = {0, 0};

        // have to maintain following maps so that Handles would not
        // delete memory once object is stopped being rendered ("removed"
        // from Renderer, that is)
        Map<geom::GeomVis*,
            Handle(AIS_Shape)> GeomLst;
        Map<geom::GeomVis*,
            Handle(AIS_Shape)> GeomTrianLst;
        Map<mesh::MeshVis*,
            Handle(MeshVS_Mesh)> MeshLst;
        Map<int, Map<int, NsolPair>> NsolLst;
    };

    RenderList::RenderList() : priv_(new RenderListPriv) {}
    RenderList::~RenderList() { SAFE_DEL(priv_); }

    void RenderList::SetCurrentGeometry(geom::GeomVis* g)
    {
        priv_->GeomLst.clear();             // v1.0: only one geom might exist
        priv_->CurrGeometry = g;
        if(g)
            priv_->GeomLst[g] = g->GetOCCModel();
    }

    void RenderList::SetCurrentTrianGeometry(geom::GeomVis* g)
    {
        priv_->GeomTrianLst.clear();             // v1.0: only one geom trian might exist
        priv_->CurrTrianGeometry = g;
        if(g)
            priv_->GeomTrianLst[g] = g->GetOCCModel();
    }

    void RenderList::SetCurrentMesh(mesh::MeshVis* m)
    {
        if(m)
            priv_->MeshLst[m] = m->GetOCCMesh();
        priv_->CurrMesh = m;
    }

    void RenderList::SetCurrentNodalSol(const int solver_id, const int dataset_id, post::NsolMeshVis* s)
    {
        if(s)
        {
            priv_->NsolLst[solver_id][dataset_id] = { s, s->GetOCCMesh() };
            priv_->CurrNsol = { solver_id, dataset_id };
        }
        else
            priv_->CurrNsol = { 0, 0 };
    }

    bool RenderList::RemoveGeometry(geom::GeomVis* g, bool* was_current)
    {
        if(was_current) *was_current = false;
        if(priv_->CurrGeometry == g)
        {
            priv_->CurrGeometry = nullptr;
            if(was_current) *was_current = true;
        }

        if(priv_->GeomLst.contains(g))
        {
            priv_->GeomLst.remove(g);
            return true;
        }

        return false;
    }

    bool RenderList::RemoveTrianGeometry(geom::GeomVis* g, bool* was_current)
    {
        if(was_current) *was_current = false;
        if(priv_->CurrTrianGeometry == g)
        {
            priv_->CurrTrianGeometry = nullptr;
            if(was_current) *was_current = true;
        }
        if(priv_->GeomTrianLst.contains(g))
        {
            priv_->GeomTrianLst.remove(g);
            return true;
        }

        return false;
    }

    bool RenderList::RemoveMesh(mesh::MeshVis* m, bool* was_current)
    {
        if(was_current) *was_current = false;
        if(priv_->CurrMesh == m)
        {
            priv_->CurrMesh = nullptr;
            if(was_current) *was_current = true;
        }
        if(priv_->MeshLst.contains(m))
        {
            priv_->MeshLst.remove(m);
            return true;
        }

        return false;
    }

    bool RenderList::RemoveNodalSol(const int solver_id, const int dataset_id, bool* was_current)
    {
        if(was_current) *was_current = false;
        if(priv_->CurrNsol.first == solver_id && priv_->CurrNsol.second == dataset_id)
        {
            priv_->CurrNsol = {0,0};
            if(was_current) *was_current = true;
        }
        if(priv_->NsolLst.contains(solver_id) && priv_->NsolLst[solver_id].contains(dataset_id))
        {
            priv_->NsolLst[solver_id].remove(dataset_id);
            if(priv_->NsolLst[solver_id].empty())
                priv_->NsolLst.remove(solver_id);
            return true;
        }

        return false;
    }

    geom::GeomVis* RenderList::GetCurrentGeometry() const { return priv_->CurrGeometry; }
    geom::GeomVis* RenderList::GetCurrentTrianGeometry() const { return priv_->CurrTrianGeometry; }
    mesh::MeshVis* RenderList::GetCurrentMesh() const { return priv_->CurrMesh; }

    post::NsolMeshVis* RenderList::GetCurrentNodalSol() const
    {
        return priv_->CurrNsol.first == 0 && priv_->CurrNsol.second == 0 ?
                    nullptr : priv_->NsolLst[priv_->CurrNsol.first][priv_->CurrNsol.second].NSol;
    }
}
