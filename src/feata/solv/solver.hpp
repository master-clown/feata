#pragma once

#include "core/comp_pluggable.hpp"
#include "plug/plugid_t.hpp"
#include "solv/solvstatus.hpp"
#include "stl/list.hpp"
#include "stl/string.hpp"
#include "util/dirty_int.hpp"


namespace plug { class PluginMgr; }
namespace post { struct DataSetInfo; }
namespace util { class ParallelTask; }

struct MeshInfo;
struct SolvInfo;
class SolverPlugin;

namespace solv
{
    class Solver : public core::CompPluggable,
                   public util::DirtyInterface      // need to resolve
    {
    public:
        Solver(const plug::PluginMgr& mesh_pmgr);
       ~Solver() override;

        bool StartSolving(util::ParallelTask*& ptask);
        void StopSolving();

        void ClearSolution();

        void SetMesh(const MeshInfo* mesh);
        void SetPlugin(const plug::plugid_t id);

        plug::plugid_t  GetPlugin() const;
        SolvInfo*       GetSolutionInfo() const;

        const List<post::DataSetInfo>& GetDataSetList() const;

        plug::PluginSettings* GetPluginSettings() const override;

    private:
        const plug::PluginMgr& solv_pmgr_;
        List<post::DataSetInfo> dsinfo_lst_;
        plug::plugid_t plug_id_ = plug::PLUGIN_ID_NULL;
        plug::PluginSettings* plug_sets_ = nullptr;
        SolvInfo* solv_info_ = nullptr;
        SolverPlugin* plug_ = nullptr;
    };
}
