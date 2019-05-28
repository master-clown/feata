#pragma once

#include "stl/list.hpp"
#include "stl/map.hpp"
#include "stl/string.hpp"
#include "plug/plugid_t.hpp"
#include "solv/solvstatus.hpp"


namespace core { class ShLibMgr;  }
namespace plug { class PluginMgr;
                 class PluginSettings;
                 struct PluginInfo; }
namespace post { struct DataSetInfo;
                 struct NodalSol; }
namespace util { class ParallelTask;  }

struct MeshInfo;
struct SolvInfo;

namespace solv
{
    class Solver;

    class FeataSolv
    {
    public:
        FeataSolv(core::ShLibMgr& lib_mgr);
       ~FeataSolv();

        bool StartSolv(const int solver_id,
                       util::ParallelTask*& ptask);
        void StopSolv();

        void OnSolvingFinished(SolvStatus status);

        void Clear();

        /**
         * The class will destroy solvers itself
         */
        bool CreateSolver(const int solver_id);
        void RemoveSolver(const int solver_id);
        void ClearSolvResults(const int solver_id);

        plug::plugid_t LoadPlugin(const String& file_name);
        bool UnloadPlugin(const plug::plugid_t id);

        void SetSolverMesh(const int solver_id, const MeshInfo* mesh);
        void SetSolverPlugin(const int solver_id,
                             const plug::plugid_t p_id);

        const MeshInfo* GetSolverMesh(const int solver_id) const;
        SolvInfo* GetSolverInfo(const int solver_id) const;
        plug::PluginMgr& GetSolvPluginMgr() const;
        plug::PluginSettings* GetSolverSettings(const int solver_id) const;

        int         GetCurrentSolverId() const;
        Solver*     GetCurrentSolver() const;
        Solver*     GetSolver(const int solver_id) const;
        List<int>   GetSolverIds() const;

        SolvStatus GetSolvingStatus(const int solver_id) const;
        bool GetSolvStatistics(const int solver_id, String& txt) const;

        const plug::PluginInfo* GetPluginInfo(const plug::plugid_t id) const;
        bool GetPluginDataSetLst(const int solver_id, const List<post::DataSetInfo>*& lst) const;
        bool GetNodalSol(const int solver_id, const int dataset_id, post::NodalSol& nsol);

        bool ContainsSolver(const int solver_id) const;

        bool IsDirty(const int solver_id);

    private:
        plug::PluginMgr* solv_pmgr_;

        int busy_solver_ = 0;   // name of solver, which is currently solving
        Map<int, Solver*> solver_lst_;
    };
}
