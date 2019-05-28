#include "solv/solver.hpp"
#include "mesh/meshinfo.hpp"
#include "plug/plugin.hpp"
#include "plug/pluginmgr.hpp"
#include "plug/pluginsets.hpp"
#include "plug/solverplugin.h"
#include "post/datasetinfo.hpp"
#include "proj/glob_events.hpp"
#include "solv/solvinfo.hpp"
#include "util/log.hpp"
#include "util/paralleltask.hpp"


using util::logger::PrintE;

namespace solv
{
    Solver::Solver(const plug::PluginMgr& mesh_pmgr)
        : solv_pmgr_(mesh_pmgr)
        , plug_sets_(new plug::PluginSettings(solv_pmgr_))
        , solv_info_(new SolvInfo())
    {}

    Solver::~Solver()
    {
        ClearSolution();
        SAFE_DEL(solv_info_);
        SAFE_DEL(plug_sets_);
    }

    bool Solver::StartSolving(util::ParallelTask*& ptask)
    {
        if(!solv_info_->Mesh || !solv_info_->Mesh->NodeNum || !solv_info_->Mesh->ElemNum)
        {
            PrintE("Cannot start solving: input mesh is null.");
            return false;
        }
        if(plug_id_ == plug::PLUGIN_ID_NULL || !solv_pmgr_.Contains(plug_id_))
        {
            PrintE("Cannot start solving: plugin is not specified or does not exist.");
            return false;
        }

        plug_ = static_cast<SolverPlugin*>(solv_pmgr_.GetById(plug_id_)->PluginPtr);

        ClearSolution();

        solv_info_->DispLst = new SolvInfo::num_t[3*solv_info_->Mesh->NodeNum] { 0 };

        plug_->SetSolvInfo(*solv_info_);

        if(!ptask)
            ptask = new util::ParallelTask();
        ptask->SetFunction([this](const QAtomicInt&) { plug_->Solve(); });
        ptask->SetOnFinishedFunc(
        [this]
        {
            const auto stat = plug_->GetSolvStatus();
            is_dirty_ = stat != SOLV_STATUS_FINISHED;

            plug_ = nullptr;

            proj::glev::SolvFinished.Raise(stat);
        });

        ptask->start();

        return true;
    }

    void Solver::StopSolving()
    {
        if(!plug_ || plug_->GetSolvStatus() != SOLV_STATUS_IN_PROGRESS)
            return;

        MakeDirty();

        plug_->CancelSolving();
    }

    void Solver::ClearSolution()
    {
        SAFE_DEL(solv_info_->DispLst);

        MakeDirty();
    }

    void Solver::SetMesh(const MeshInfo* mesh)
    { solv_info_->Mesh = mesh; }

    void Solver::SetPlugin(const plug::plugid_t id)
    {
        if(plug_id_ != plug::PLUGIN_ID_NULL)
            ClearSolution();

        plug_sets_->Clear();
        plug_sets_->SetPluginId(plug_id_ = id);

        MakeDirty();

        if(id == plug::PLUGIN_ID_NULL)
        {
            return;
        }

        plug_sets_->ParseFromPlugin();

        auto ds_lst = String(static_cast<SolverPlugin*>(solv_pmgr_.GetById(id)->PluginPtr)->GetNodalDataSetList()).split("~");

        dsinfo_lst_.clear();
        for(const auto& ds: ds_lst)
        {
            const auto parts = ds.split("$");

            dsinfo_lst_.append(post::DataSetInfo{ parts[0].toInt(), parts[1], parts[2] });
        }
    }

    plug::plugid_t Solver::GetPlugin() const { return plug_id_; }
    SolvInfo*      Solver::GetSolutionInfo() const { return solv_info_; }
    const List<post::DataSetInfo>& Solver::GetDataSetList() const { return dsinfo_lst_; }
    plug::PluginSettings* Solver::GetPluginSettings() const { return plug_sets_; }
}
