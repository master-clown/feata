#include "mesh/featamesh.hpp"
#include "mesh/mesher.hpp"
#include "mesh/meshvis.hpp"
#include "geom/mdl/shapetriang.hpp"
#include "gui/renderlist.hpp"
#include "plug/pluginmgr.hpp"
#include "plug/mesherplugin.h"
#include "util/log.hpp"
#include "util/paralleltask.hpp"
#include "proj/glob_events.hpp"


using util::logger::PrintE;

namespace mesh
{
    FeataMesh::FeataMesh(core::ShLibMgr& lib_mgr)
        : mesh_pmgr_(new plug::PluginMgr(lib_mgr))
    {
        // can discard here
        uint tmp = proj::glev::MeshingFinished.AddFollower(
        [this](MeshingStatus status)
        {
            OnMeshingFinished(status);
        });
        UNUSED_PARAM(tmp);
    }

    FeataMesh::~FeataMesh()
    {
        Clear();
        SAFE_DEL(mesh_pmgr_);
    }

    bool FeataMesh::StartMeshing(const int mesher_id,
                                 util::ParallelTask*& ptask)
    {
        if(!curr_geom_ || !curr_geom_->IsValid())
        {
            PrintE("Cannot start meshing: triangle geometry is not available or is invalid.");
            return false;
        }
        if(!ContainsMesher(mesher_id))
        {
            PrintE("Cannot start meshing: mesher '%1' does not exist."_qs.arg(mesher_id));
            return false;
        }

        if(!mesher_lst_[mesher_id].mesher->StartMeshing(curr_geom_, ptask))
            return false;

        busy_mesher_ = mesher_id;

        return true;
    }

    void FeataMesh::StopMeshing()
    {
        mesher_lst_[busy_mesher_].mesher->StopMeshing();
    }

    void FeataMesh::OnMeshingFinished(MeshingStatus status)
    {
        if(status != MESHING_STATUS_FINISHED)
        {
            busy_mesher_ = 0;
            return;
        }

        mesher_lst_[busy_mesher_].mesh_vis->Init(mesher_lst_[busy_mesher_].mesher->GetMesh());

        busy_mesher_ = 0;
    }

    void FeataMesh::Clear()
    {
        const auto name_lst { GetMesherNames() };
        for(const auto& n: name_lst)
            RemoveMesher(n);
    }

    bool FeataMesh::CreateMesher(const int mesher_id)
    {
        if(ContainsMesher(mesher_id))
        {
            PrintE("Mesher '%1' already exists."_qs.arg(mesher_id));
            return false;
        }

        mesher_lst_[mesher_id] = { new Mesher(*mesh_pmgr_), new MeshVis() };
        return true;
    }

    void FeataMesh::RemoveMesher(const int mesher_id)
    {
        if(!ContainsMesher(mesher_id))
            return;

        delete mesher_lst_[mesher_id].mesh_vis;
        delete mesher_lst_[mesher_id].mesher;
        mesher_lst_.remove(mesher_id);
    }

    void FeataMesh::ClearMeshResults(const int mesher_id)
    {
        if(!ContainsMesher(mesher_id))
            return;

        mesher_lst_[mesher_id].mesh_vis->Clear();
        mesher_lst_[mesher_id].mesher->ClearMesh();
    }

    void FeataMesh::SetCurrentGeometry(geom::mdl::ShapeTriangulation* geom)
    { curr_geom_ = geom; }

    void FeataMesh::SetMesherPlugin(const int mesher_id,
                                    const plug::plugid_t p_id)
    {
        if(!ContainsMesher(mesher_id))
        {
            PrintE("Mesher '%1' does not exist."_qs.arg(mesher_id));
            return;
        }

        mesher_lst_[mesher_id].mesher->SetPlugin(p_id);
    }

    plug::plugid_t FeataMesh::LoadPlugin(const String& file_name)
    {
        const auto pid = mesh_pmgr_->LoadPlugin(file_name);

        if(pid && mesh_pmgr_->GetById(pid)->Type != plug::PLUGIN_TYPE_MESHER)
        {
            PrintE("Plugin '%1' is not a mesher plugin.");
            mesh_pmgr_->UnloadPlugin(pid);
            return plug::PLUGIN_ID_NULL;
        }

        return pid;
    }

    bool FeataMesh::UnloadPlugin(const plug::plugid_t id)
    {
        if(id == plug::PLUGIN_ID_NULL)
            return false;

        // firstly need to remove plugin's mentions from all meshers.
        for(auto& c: mesher_lst_)
            if(c.mesher->GetPlugin() == id)
                c.mesher->SetPlugin(plug::PLUGIN_ID_NULL);

        return mesh_pmgr_->UnloadPlugin(id);
    }

    geom::mdl::ShapeTriangulation* FeataMesh::GetCurrentGeometry() const
    { return curr_geom_; }

    plug::PluginMgr&      FeataMesh::GetMeshPluginMgr() const { return *mesh_pmgr_; }
    plug::PluginSettings* FeataMesh::GetMesherSettings(const int mesher_id) const
    { return GetMesher(mesher_id)->GetPluginSettings(); }

    bool FeataMesh::HasMeshVis(const int mesher_id) const
    { return ContainsMesher(mesher_id) && mesher_lst_[mesher_id].mesh_vis->GetOCCMesh(); }

    int FeataMesh::GetCurrentMesherId() const { return busy_mesher_; }

    Mesher* FeataMesh::GetCurrentMesher() const
    { return busy_mesher_ != 0 ? mesher_lst_[busy_mesher_].mesher : nullptr; }

    Mesher* FeataMesh::GetMesher(const int mesher_id) const
    { return ContainsMesher(mesher_id) ? mesher_lst_[mesher_id].mesher : nullptr; }

    MeshInfo* FeataMesh::GetMesherRes(const int mesher_id) const
    { return ContainsMesher(mesher_id) ? mesher_lst_[mesher_id].mesher->GetMesh() : nullptr; }

    MeshVis* FeataMesh::GetMeshVis(const int mesher_id) const
    { return mesher_lst_[mesher_id].mesh_vis; }

    List<int> FeataMesh::GetMesherNames() const { return mesher_lst_.keys(); }

    MeshingStatus FeataMesh::GetMeshingStatus(const int mesher_id) const
    {
        auto pl = mesh_pmgr_->GetById(mesher_lst_[mesher_id].mesher->GetPlugin());
        if(!pl || pl->IsNull())
            return MESHING_STATUS_NOT_STARTED;

        return static_cast<MesherPlugin*>(pl->PluginPtr)->GetMeshingStatus();
    }

    bool FeataMesh::GetMeshStatistics(const int mesher_id,
                                      const bool detailed,
                                      String& txt) const
    {
        const auto& mshr = mesher_lst_[mesher_id].mesher;
        auto pl = mesh_pmgr_->GetById(mshr->GetPlugin());
        if(!pl || pl->IsNull() || !mshr->GetMesh() || !mshr->GetMesh()->ElemNum)
            return false;

        const String detail = static_cast<MesherPlugin*>(pl->PluginPtr)->GetMeshStats();

        txt = "Basic information:\n"
              "\tNumber of nodes: %1\n"
              "\tNumber of elements: %2\n%3"_qs
              .arg(mshr->GetMesh()->NodeNum)
              .arg(mshr->GetMesh()->ElemNum)
              .arg(detail.isEmpty() || !detailed ? "" : ("\nDetailed information and log:\n" + detail));

        return true;
    }

    bool FeataMesh::ContainsMesher(const int mesher_id) const
    { return mesher_lst_.contains(mesher_id); }

    const plug::PluginInfo* FeataMesh::GetPluginInfo(const plug::plugid_t id) const
    {
        if(!mesh_pmgr_->Contains(id))
            return nullptr;

        plug::Plugin* pl;
        mesh_pmgr_->GetById(id, pl);

        return &pl->Info;
    }

    bool FeataMesh::IsDirty(const int mesher_id) { return GetMesher(mesher_id)->IsDirty(); }
}
