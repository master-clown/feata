#include "solv/featasolv.hpp"
#include "mesh/meshinfo.hpp"
#include "plug/pluginmgr.hpp"
#include "plug/solverplugin.h"
#include "solv/solver.hpp"
#include "solv/solvinfo.hpp"
#include "util/log.hpp"
#include "util/paralleltask.hpp"
#include "proj/glob_events.hpp"
#include "post/datasetinfo.hpp"
#include "post/nodalsol.hpp"


using util::logger::PrintE;

namespace solv
{
    FeataSolv::FeataSolv(core::ShLibMgr& lib_mgr)
        : solv_pmgr_(new plug::PluginMgr(lib_mgr))
    {
        // can discard here
        uint tmp = proj::glev::SolvFinished.AddFollower(
        [this](SolvStatus status)
        {
            OnSolvingFinished(status);
        });
        UNUSED_PARAM(tmp);
    }

    FeataSolv::~FeataSolv()
    {
        Clear();
        SAFE_DEL(solv_pmgr_);
    }

    bool FeataSolv::StartSolv(const int solver_id,
                              util::ParallelTask*& ptask)
    {
        if(!ContainsSolver(solver_id))
        {
            PrintE("Cannot start solving: solver '%1' does not exist."_qs.arg(solver_id));
            return false;
        }

        if(!solver_lst_[solver_id]->StartSolving(ptask))
            return false;

        busy_solver_ = solver_id;

        return true;
    }

    void FeataSolv::StopSolv()
    {
        solver_lst_[busy_solver_]->StopSolving();
    }

    void FeataSolv::OnSolvingFinished(SolvStatus)
    {
        busy_solver_ = 0;
    }

    void FeataSolv::Clear()
    {
        const auto name_lst { GetSolverIds() };
        for(const auto& n: name_lst)
            RemoveSolver(n);
    }

    bool FeataSolv::CreateSolver(const int solver_id)
    {
        if(ContainsSolver(solver_id))
        {
            PrintE("Solver '%1' already exists."_qs.arg(solver_id));
            return false;
        }

        solver_lst_[solver_id] = new Solver(*solv_pmgr_);
        return true;
    }

    void FeataSolv::RemoveSolver(const int solver_id)
    {
        if(!ContainsSolver(solver_id))
            return;

        delete solver_lst_[solver_id];
        solver_lst_.remove(solver_id);
    }

    void FeataSolv::ClearSolvResults(const int solver_id)
    {
        if(!ContainsSolver(solver_id))
            return;

        solver_lst_[solver_id]->ClearSolution();
    }

    void FeataSolv::SetSolverMesh(const int solver_id, const MeshInfo* mesh)
    { if(ContainsSolver(solver_id)) solver_lst_[solver_id]->SetMesh(mesh); }

    void FeataSolv::SetSolverPlugin(const int solver_id,
                                    const plug::plugid_t p_id)
    {
        if(!ContainsSolver(solver_id))
        {
            PrintE("Solver '%1' does not exist."_qs.arg(solver_id));
            return;
        }

        solver_lst_[solver_id]->SetPlugin(p_id);
    }

    plug::plugid_t FeataSolv::LoadPlugin(const String& file_name)
    {
        const auto pid = solv_pmgr_->LoadPlugin(file_name);

        if(pid && solv_pmgr_->GetById(pid)->Type != plug::PLUGIN_TYPE_SOLVER)
        {
            PrintE("Plugin '%1' is not a solver plugin.");
            solv_pmgr_->UnloadPlugin(pid);
            return plug::PLUGIN_ID_NULL;
        }

        return pid;
    }

    bool FeataSolv::UnloadPlugin(const plug::plugid_t id)
    {
        if(id == plug::PLUGIN_ID_NULL)
            return false;

        // firstly need to remove plugin's dependencies from all solvers.
        for(auto& c: solver_lst_)
            if(c->GetPlugin() == id)
                c->SetPlugin(plug::PLUGIN_ID_NULL);

        return solv_pmgr_->UnloadPlugin(id);
    }

    const MeshInfo* FeataSolv::GetSolverMesh(const int solver_id) const
    { return ContainsSolver(solver_id) ? solver_lst_[solver_id]->GetSolutionInfo()->Mesh : nullptr; }

    SolvInfo* FeataSolv::GetSolverInfo(const int solver_id) const
    { return ContainsSolver(solver_id) ? solver_lst_[solver_id]->GetSolutionInfo() : nullptr; }

    plug::PluginMgr&      FeataSolv::GetSolvPluginMgr() const { return *solv_pmgr_; }
    plug::PluginSettings* FeataSolv::GetSolverSettings(const int solver_id) const
    { return ContainsSolver(solver_id) ? GetSolver(solver_id)->GetPluginSettings() : nullptr; }

    int FeataSolv::GetCurrentSolverId() const { return busy_solver_; }

    Solver* FeataSolv::GetCurrentSolver() const
    { return busy_solver_ != 0 ? solver_lst_[busy_solver_] : nullptr; }

    Solver* FeataSolv::GetSolver(const int solver_id) const
    { return ContainsSolver(solver_id) ? solver_lst_[solver_id] : nullptr; }

    List<int> FeataSolv::GetSolverIds() const { return solver_lst_.keys(); }

    SolvStatus FeataSolv::GetSolvingStatus(const int solver_id) const
    {
        auto pl = solv_pmgr_->GetById(solver_lst_[solver_id]->GetPlugin());
        if(!pl || pl->IsNull())
            return SOLV_STATUS_NOT_STARTED;

        return static_cast<SolverPlugin*>(pl->PluginPtr)->GetSolvStatus();
    }

    bool FeataSolv::GetSolvStatistics(const int solver_id, String& txt) const
    {
        const auto& solv = solver_lst_[solver_id];
        auto pl = solv_pmgr_->GetById(solv->GetPlugin());
        if(!pl || pl->IsNull() || !solv->GetSolutionInfo() || !solv->GetSolutionInfo()->Mesh)
            return false;

        const String detail = static_cast<SolverPlugin*>(pl->PluginPtr)->GetSolvStats();

        txt = detail.isEmpty() ? "Solving information is not available."
                               : ("\nDetailed information:\n" + detail);

        return true;
    }

    bool FeataSolv::ContainsSolver(const int solver_id) const
    { return solver_lst_.contains(solver_id); }

    const plug::PluginInfo* FeataSolv::GetPluginInfo(const plug::plugid_t id) const
    {
        if(!solv_pmgr_->Contains(id))
            return nullptr;

        plug::Plugin* pl;
        solv_pmgr_->GetById(id, pl);

        return &pl->Info;
    }

    bool FeataSolv::GetPluginDataSetLst(const int solver_id, const List<post::DataSetInfo>*& lst) const
    {
        if(!ContainsSolver(solver_id))
            return false;

        lst = &solver_lst_[solver_id]->GetDataSetList();

        return true;
    }

    bool FeataSolv::GetNodalSol(const int solver_id, const int dataset_id, post::NodalSol& nsol)
    {
        if(!ContainsSolver(solver_id))
            return false;

        plug::Plugin* pl;
        solv_pmgr_->GetById(solver_lst_[solver_id]->GetPlugin(), pl);

        nsol.Sol = GetSolverInfo(solver_id);
        if(!nsol.Sol->DispLst)
            return false;

        nsol.DataSet.resize(nsol.Sol->Mesh->NodeNum);

        if(dataset_id == 0) // need total disp
        {
            for(auto i = 0u; i < nsol.Sol->Mesh->NodeNum; ++i)
                nsol.DataSet[i] = fabs(nsol.Sol->DispLst[3*i + 0]) +
                                  fabs(nsol.Sol->DispLst[3*i + 1]) +
                                  fabs(nsol.Sol->DispLst[3*i + 2]);

            return true;
        }

        return !static_cast<SolverPlugin*>(pl->PluginPtr)->GetNodalDataSet(dataset_id, &nsol.DataSet[0]);
    }

    bool FeataSolv::IsDirty(const int solver_id) { return GetSolver(solver_id)->IsDirty(); }
}
