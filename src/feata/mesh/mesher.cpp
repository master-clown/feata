#include "mesh/mesher.hpp"
#include "mesh/meshinfo.hpp"
#include "geom/mdl/shapetriang.hpp"
#include "plug/mesherplugin.h"
#include "plug/plugin.hpp"
#include "plug/pluginmgr.hpp"
#include "plug/pluginsets.hpp"
#include "proj/glob_events.hpp"
#include "util/log.hpp"
#include "util/paralleltask.hpp"


using util::logger::PrintE;

namespace mesh
{
    Mesher::Mesher(const plug::PluginMgr& mesh_pmgr)
        : mesh_pmgr_(mesh_pmgr)
        , plug_sets_(new plug::PluginSettings(mesh_pmgr_))
    {}

    Mesher::~Mesher()
    {
        ClearMesh();
        SAFE_DEL(plug_sets_);
    }

    bool Mesher::StartMeshing(geom::mdl::ShapeTriangulation* geom,
                              util::ParallelTask*& ptask)
    {
        if(!geom->IsValid())
        {
            PrintE("Cannot start meshing: missing or invalid triangulation.");
            return false;
        }
        if(plug_id_ == plug::PLUGIN_ID_NULL || !mesh_pmgr_.Contains(plug_id_))
        {
            PrintE("Cannot start meshing: plugin is not specified or does not exist.");
            return false;
        }

        plug_ = static_cast<MesherPlugin*>(mesh_pmgr_.GetById(plug_id_)->PluginPtr);

        ClearMesh();

         in_mesh_ = new MeshInfo();
        out_mesh_ = new MeshInfo();
        geom->ToMeshInfo(*in_mesh_);
        plug_->SetInputMeshInfo(*in_mesh_);
        plug_->SetOutputMeshInfo(*out_mesh_);

        if(!ptask)
            ptask = new util::ParallelTask();
        ptask->SetFunction([this](const QAtomicInt&) { plug_->Mesh(); });
        ptask->SetOnFinishedFunc(
        [this]
        {
            const auto stat = plug_->GetMeshingStatus();
            is_dirty_ = stat != MESHING_STATUS_FINISHED;

            plug_ = nullptr;

            proj::glev::MeshingFinished.Raise(stat);
        });

        ptask->start();

        return true;
    }

    void Mesher::StopMeshing()
    {
        if(!plug_ || plug_->GetMeshingStatus() != MESHING_STATUS_IN_PROGRESS)
            return;

        MakeDirty();

        plug_->CancelMeshing();
    }

    void Mesher::ClearMesh()
    {
        SAFE_DEL(in_mesh_);

        if(out_mesh_ && plug_id_ != plug::PLUGIN_ID_NULL)
            static_cast<MesherPlugin*>(mesh_pmgr_.GetById(plug_id_)->PluginPtr)->FreeOutputMeshInfo(*out_mesh_);
        SAFE_DEL(out_mesh_);

        MakeDirty();
    }

    void Mesher::SetPlugin(const plug::plugid_t id)
    {
        if(plug_id_ != plug::PLUGIN_ID_NULL)
            ClearMesh();

        plug_sets_->Clear();
        plug_sets_->SetPluginId(plug_id_ = id);
        if(id != plug::PLUGIN_ID_NULL)
            plug_sets_->ParseFromPlugin();

        MakeDirty();
    }

    plug::plugid_t  Mesher::GetPlugin() const { return plug_id_; }
    MeshInfo*       Mesher::GetMesh() const { return out_mesh_; }

    plug::PluginSettings* Mesher::GetPluginSettings() const { return plug_sets_; }
}
