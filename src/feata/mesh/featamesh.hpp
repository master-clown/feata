#pragma once

#include "stl/list.hpp"
#include "stl/map.hpp"
#include "stl/string.hpp"
#include "plug/plugid_t.hpp"
#include "mesh/meshingstatus.hpp"


namespace core { class ShLibMgr;  }
namespace geom::mdl { struct ShapeTriangulation; }
namespace plug { class PluginMgr;
                 class PluginSettings;
                 struct PluginInfo; }
namespace util { class ParallelTask;  }

struct MeshInfo;

namespace mesh
{
    class Mesher;
    class MeshVis;

    class FeataMesh
    {
    public:
        FeataMesh(core::ShLibMgr& lib_mgr);
       ~FeataMesh();

        bool StartMeshing(const int mesher_id,
                          util::ParallelTask*& ptask);
        void StopMeshing();

        void OnMeshingFinished(MeshingStatus status);

        void Clear();
        /**
         * The class will destroy meshers itself
         */
        bool CreateMesher(const int mesher_id);
        void RemoveMesher(const int mesher_id);
        void ClearMeshResults(const int mesher_id);

        plug::plugid_t LoadPlugin(const String& file_name);
        bool UnloadPlugin(const plug::plugid_t id);

        void SetCurrentGeometry(geom::mdl::ShapeTriangulation* geom);
        void SetMesherPlugin(const int mesher_id,
                             const plug::plugid_t p_id);        

        geom::mdl::ShapeTriangulation* GetCurrentGeometry() const;
        plug::PluginMgr& GetMeshPluginMgr() const;
        plug::PluginSettings* GetMesherSettings(const int mesher_id) const;

        bool HasMeshVis(const int mesher_id) const;

        int         GetCurrentMesherId() const;
        Mesher*     GetCurrentMesher() const;
        Mesher*     GetMesher(const int mesher_id) const;
        MeshInfo*   GetMesherRes(const int mesher_id) const;
        MeshVis*    GetMeshVis(const int mesher_id) const;
        List<int>   GetMesherNames() const;

        MeshingStatus GetMeshingStatus(const int mesher_id) const;
        bool GetMeshStatistics(const int mesher_id, String& txt) const;

        const plug::PluginInfo* GetPluginInfo(const plug::plugid_t id) const;

        bool ContainsMesher(const int mesher_id) const;

        bool IsDirty(const int mesher_id);

    private:
        struct MeshComp_
        {
            Mesher* mesher = nullptr;
            MeshVis* mesh_vis = nullptr;
        };

        geom::mdl::ShapeTriangulation* curr_geom_ = nullptr;
        plug::PluginMgr* mesh_pmgr_;

        int busy_mesher_ = 0;   // name of mesher, which is currently generating mesh
        Map<int, MeshComp_> mesher_lst_;
    };
}
