#pragma once

#include <QMainWindow>
#include "core/comp_def.hpp"
#include "mesh/meshingstatus.hpp"
#include "solv/solvstatus.hpp"
#include "plug/plugid_t.hpp"
#include "stl/string.hpp"


namespace Ui        { class MainWindow; }
namespace core      { class ShLibMgr; }
namespace gui       { class TreeWidgetItem; }
namespace gui::wgt  { class WidgetMgr; }
namespace post      { struct DataSetInfo; }
namespace proj      { class Project; }
namespace util      { class ParallelTask; }

class QTreeWidgetItem;
class QProgressBar;
class QPushButton;

namespace gui
{
    enum MainWindowState : int
    {
        MAIN_WND_STATE_SHOW_SHAPE = 1 << 0,
        MAIN_WND_STATE_SHOW_TRIAN = 1 << 1,
        MAIN_WND_STATE_SHOW_MESH  = 1 << 2,
        MAIN_WND_STATE_SHOW_SOLU  = 1 << 3,
        MAIN_WND_STATE_SHOW_ENUMMASK  = MAIN_WND_STATE_SHOW_SHAPE |
                                        MAIN_WND_STATE_SHOW_TRIAN |
                                        MAIN_WND_STATE_SHOW_MESH |
                                        MAIN_WND_STATE_SHOW_SOLU,
        MAIN_WND_STATE_LOCKED = 1 << 4
    };
    MAKE_ENUM_FLAGGED(MainWindowState, int);

    // item order here and in Designer must match
    enum ProjectTreeMainItem
    {
        PROJ_TREE_MAIN_ITEM_ROOT,
        PROJ_TREE_MAIN_ITEM_GEOM,
        PROJ_TREE_MAIN_ITEM_GEOM_TRIAN,
        PROJ_TREE_MAIN_ITEM_MESH,
        PROJ_TREE_MAIN_ITEM_MESH_PLUGIN,
        PROJ_TREE_MAIN_ITEM_SOLVER,
        PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN,
        PROJ_TREE_MAIN_ITEM_RESULT,
        PROJ_TREE_MAIN_ITEM_COUNT
    };


    class MainWindow
            : public QMainWindow
    {
    public:
        explicit MainWindow(QWidget *parent = nullptr);
       ~MainWindow() override;

        /**
         * AddWndState, RemWndState
         *
         *     The main functions of the GUI component!
         */
        void AddWndState(const MainWindowState new_states);
        void RemWndState(const MainWindowState rem_states);

        bool IsGuiLocked() const;

    signals:
        void InitProgressBarSig(const int min,
                                const int max);
        void SetProgressValueSig(const int val);
        void HideProgressBarSig();

    public slots:
        void ChangeStatusMsg(const String& msg,
                             const uint timeout);

    protected:
        void closeEvent(QCloseEvent* ev) override;

    private slots:
        void LockGui();
        void UnlockGui();

        // both zeros will make "BUSY" bar
        void InitProgressBar(const int min,
                             const int max);
        void SetProgressValue(const int val);
        void HideProgressBar();

        void OnStartBtnClicked();
        void OnStopBtnClicked();

        void StartTriang();
        void StartMeshing();
        void StartSolving();
        void OnTriangFinished();
        void OnMeshingFinished(const MeshingStatus stat);
        void OnSolvingFinished(const SolvStatus st);

        void LoadPlugin(const core::ComponentPluggable comp);
        void UnloadPlugin(gui::TreeWidgetItem* item, bool by_user = true);

        // when a plugin is deleted, for instance
        void UpdatePluggableInstances(const core::ComponentPluggable comp,
                                      const plug::plugid_t pid);
        void ActiveCompBecameDirty();

        void RefillPluginComboBox(const core::ComponentPluggable comp);
        void RefillCompComboBox(const core::ComponentPluggable comp);
        void OnPluginComboBoxSelChanged(int idx);
        void OnCompComboBoxSelChanged(int idx);

        void OnProjectTreeLClick();
        void OnProjectTreeRClick(const QPoint& pnt);

        void OnTreeProjUnload();
        void OnTreeGeomLoad();
        void OnTreeGeomTrianCreate();
        void OnTreeGeomTrianInstDelete(TreeWidgetItem* item, const bool by_user = true);
        void OnTreeMeshInstCreate();
        void OnTreeMeshInstDelete(TreeWidgetItem* item, const bool by_user = true);
        void OnTreeMeshInstPrintStats(TreeWidgetItem* item);
        void OnTreeSolvInstCreate();
        void OnTreeSolvInstDelete(TreeWidgetItem* item, const bool by_user = true);
        void OnTreeSolvInstPrintStats(TreeWidgetItem* item);
        void OnTreePostInstAddNodalSol(TreeWidgetItem* item, const post::DataSetInfo& ds);

        void OnTreeItemChanged(QTreeWidgetItem* item, const int col);

        void OnMenuFileOpen();
        void OnMenuFileCloseCurrent();
        void OnMenuFileExportMesh();
        void OnMenuFileExportResults();
        void OnMenuViewSetDefaultView();
        void OnMenuViewBackground();
        void OnMenuMeshCreate();

        void OnToolSelectRect();
        void OnToolUnselectRect();
        void OnToolViewXYZ();
        void OnToolViewXY();
        void OnToolViewYZ();
        void OnToolViewXZ();
        void OnToolViewMirror();
        void OnToolViewSun();
        void OnToolViewWireframe();

    private:
        Ui::MainWindow *ui;
        core::ShLibMgr* shlib_mgr_;
        proj::Project* proj_;
        wgt::WidgetMgr* wgt_mgr_;

        MainWindowState wnd_state_;
        QTreeWidgetItem* tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_COUNT];
        QPushButton* btn_stop_op_;
        QProgressBar* progress_bar_;

       // misc
        String tree_item_name_before_edit_;
        TreeWidgetItem* active_item_ = nullptr;     // currently selected item

        // most recently selected instances. Index with core::ComponentPluggable
        TreeWidgetItem* active_inst_lst_[core::COMPONENT_PLUGGABLE_ENUM_COUNT] = { 0 };

        util::ParallelTask* curr_par_task_ = nullptr; // to be able to stop it

       // methods

        void InitDesign();
        void InitMenu();
        void InitEventFollow();

        void UnloadGeometry();
        void UnloadTriang();
        void UnloadMesh();
        void UnloadMeshPlugs();
        void UnloadSolver();        // here all results will be cleared as well
        void UnloadSolverPlugs();

        void ShowActiveMesher(const bool is_result);
        void ShowActiveSolver();
        void ShowActivePostpr();

        uint CountSimiliarNames(const TreeWidgetItem* item,
                                const String& name,
                                bool* has_exact = nullptr,
                                uint* max_avail_num = nullptr) const;
        // just appends to base_name a number
        String GenerateItemName(const TreeWidgetItem* item,
                                const String& base_name) const;

        void FillTreeItemContextMenuRoot(QMenu& menu);
        void FillTreeItemContextMenuGeom(QMenu& menu);
        //void FillTreeItemContextMenuGeomComp(QMenu& menu, TreeWidgetItem* item);
        void FillTreeItemContextMenuGeomTrian(QMenu& menu);
        void FillTreeItemContextMenuGeomTrianInst(QMenu& menu, TreeWidgetItem* item);
        void FillTreeItemContextMenuMesh(QMenu& menu);
        void FillTreeItemContextMenuMeshInst(QMenu& menu, TreeWidgetItem* item);
        void FillTreeItemContextMenuSolv(QMenu& menu);
        void FillTreeItemContextMenuSolvInst(QMenu& menu, TreeWidgetItem* item);
        void FillTreeItemContextMenuPost(QMenu& menu);
        void FillTreeItemContextMenuPostInst(QMenu& menu, TreeWidgetItem* item);
        void FillTreeItemContextMenuPlugin(QMenu& menu, const core::ComponentPluggable comp);
        void FillTreeItemContextMenuPluginInst(QMenu& menu, TreeWidgetItem* item);
        void FillTreeItemContextMenuGenericComp(QMenu& menu, QTreeWidgetItem* item);

        void LoadSettings();
        void SaveSettings();

        Q_OBJECT
    };
}
