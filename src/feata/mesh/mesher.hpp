#pragma once

#include "core/comp_pluggable.hpp"
#include "mesh/meshingstatus.hpp"
#include "plug/plugid_t.hpp"
#include "stl/string.hpp"
#include "util/dirty_int.hpp"


namespace geom::mdl { struct ShapeTriangulation; }
namespace plug      { class PluginMgr; }
namespace util      { class ParallelTask; }

struct MeshInfo;
class MesherPlugin;

namespace mesh
{
    class Mesher : public core::CompPluggable,
                   public util::DirtyInterface      // need to remesh
    {
    public:
        Mesher(const plug::PluginMgr& mesh_pmgr);
       ~Mesher() override;

        bool StartMeshing(geom::mdl::ShapeTriangulation* geom,
                          util::ParallelTask*& ptask);
        void StopMeshing();

        void ClearMesh();

        void SetPlugin(const plug::plugid_t id);

        plug::plugid_t  GetPlugin() const;
        MeshInfo*       GetMesh() const;

        plug::PluginSettings* GetPluginSettings() const override;

    private:
        const plug::PluginMgr& mesh_pmgr_;
        plug::plugid_t plug_id_ = plug::PLUGIN_ID_NULL;
        plug::PluginSettings* plug_sets_ = nullptr;
        MeshInfo*  in_mesh_ = nullptr;
        MeshInfo* out_mesh_ = nullptr;
        MesherPlugin* plug_ = nullptr;
    };
}
