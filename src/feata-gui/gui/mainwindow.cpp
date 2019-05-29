#include "gui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include "core/shlib_mgr.hpp"
#include "geom/featageom.hpp"
#include "geom/mdl/shapetriang.hpp"
#include "gui/renderer.hpp"
#include "gui/renderlist.hpp"
#include "gui/dlg/filedlg.hpp"
#include "gui/dlg/mboxdlg.hpp"
#include "gui/wgt/widgetmgr.hpp"
#include "gui/wnd/logwidget.hpp"
#include "mesh/featamesh.hpp"
#include "plug/plugininfo.hpp"
#include "post/datasetinfo.hpp"
#include "post/featapost.hpp"
#include "post/nodalsol.hpp"
#include "proj/glob_events.hpp"
#include "proj/glob_obj.hpp"
#include "proj/project.hpp"
#include "solv/featasolv.hpp"
#include "stl/vector.hpp"
#include "test/test.hpp"
#include "util/log.hpp"
#include "util/paralleltask.hpp"
#include "gui/util/treewidgetitem.hpp"

#define SETTINGS_FILE_NAME "settings.ini"


using util::logger::Print;
using util::logger::PrintE;

namespace gui
{
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
        , shlib_mgr_(new core::ShLibMgr())
    {
        ui->setupUi(this);

        util::logger::Init({true, util::logger::LOG_TYPE_WIDGET, {}, ui->log});

        proj::globj::gProject = proj_ =
            new proj::Project(*shlib_mgr_);

        wgt_mgr_ = new wgt::WidgetMgr(ui->detailWidget,
                                      ui->detailTitleWidget,
                                      ui->lblDetailTitle,
                                      ui->btnDetailCmd,
                                      ui->gbHeaderCompPlugin,
                                      ui->gbHeaderDepComp);

        InitDesign();
        InitEventFollow();
        LoadSettings();

//        plug::PluginSettings sets(util::filesys::ReadFile("../test/pluginsets.json"));

//        wgt_detail = new wgt::WidgetSetsCont(sets);
//        ui->detailWidget->layout()->addWidget(wgt_detail);
    }

    MainWindow::~MainWindow()
    {
        SAFE_DEL(btn_stop_op_);
        SAFE_DEL(progress_bar_);
        SAFE_DEL(wgt_mgr_);
        SAFE_DEL(proj_);
        proj::globj::gProject = nullptr;
        SAFE_DEL(shlib_mgr_);
        delete ui;
    }

    void MainWindow::AddWndState(const MainWindowState new_states)
    {
        const auto true_new_states = (wnd_state_ ^ new_states) & new_states;

        if(true_new_states & MAIN_WND_STATE_LOCKED)
        {
            ui->projectTree->setEnabled(false);
            ui->menubar->setEnabled(false);
            ui->dockWidget_3->setEnabled(false);
        }
        if(true_new_states & MAIN_WND_STATE_SHOW_SHAPE)
        {
            ui->renderView->GetRenderer()->AddToScene(RENDER_OBJ_TYPE_SHAPE);
        }
        if(true_new_states & MAIN_WND_STATE_SHOW_TRIAN)
        {
            ui->renderView->GetRenderer()->AddToScene(RENDER_OBJ_TYPE_TRIAN);
        }
        if(true_new_states & MAIN_WND_STATE_SHOW_MESH)
        {
            ui->renderView->GetRenderer()->AddToScene(RENDER_OBJ_TYPE_MESH);
        }
        if(true_new_states & MAIN_WND_STATE_SHOW_SOLU)
        {
            ui->renderView->GetRenderer()->AddToScene(RENDER_OBJ_TYPE_SOLU);
        }

        wnd_state_ |= new_states;
    }

    void MainWindow::RemWndState(const MainWindowState rem_states)
    {
        const auto true_rem = wnd_state_ & rem_states;

        if(true_rem & MAIN_WND_STATE_LOCKED)
        {
            ui->projectTree->setEnabled(true);
            ui->menubar->setEnabled(true);
            ui->dockWidget_3->setEnabled(true);
        }
        if(true_rem & MAIN_WND_STATE_SHOW_SHAPE)
        {
            ui->renderView->GetRenderer()->RemFromScene(RENDER_OBJ_TYPE_SHAPE);
        }
        if(true_rem & MAIN_WND_STATE_SHOW_TRIAN)
        {
            ui->renderView->GetRenderer()->RemFromScene(RENDER_OBJ_TYPE_TRIAN);
        }
        if(true_rem & MAIN_WND_STATE_SHOW_MESH)
        {
            ui->renderView->GetRenderer()->RemFromScene(RENDER_OBJ_TYPE_MESH);
        }
        if(true_rem & MAIN_WND_STATE_SHOW_SOLU)
        {
            ui->renderView->GetRenderer()->RemFromScene(RENDER_OBJ_TYPE_SOLU);
        }

        wnd_state_ &= ~rem_states;
    }

    bool MainWindow::IsGuiLocked() const
    {
        return wnd_state_ & MAIN_WND_STATE_LOCKED;
    }

    void MainWindow::InitDesign()
    {
        setWindowIcon(QIcon("feata.ico"));
        setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

        int i_it = 0;
        for(QTreeWidgetItemIterator it(ui->projectTree); *it; ++it)
            tree_main_item_lst_[i_it++] = *it;
        tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_ROOT]->setExpanded(true);

        connect(ui->btnDetailCmd, &QPushButton::clicked, this, &MainWindow::OnStartBtnClicked);

        progress_bar_ = new QProgressBar();
        progress_bar_->setVisible(false);
        progress_bar_->setFixedWidth(200);

        btn_stop_op_ = new QPushButton(QIcon(":/img/shared/img/stop.png"), "");
        btn_stop_op_->setVisible(false);
        connect(btn_stop_op_, &QPushButton::clicked, this, &MainWindow::OnStopBtnClicked);

        ui->statusbar->addPermanentWidget(btn_stop_op_);
        ui->statusbar->addPermanentWidget(progress_bar_);

        ui->detailTitleWidget->setVisible(false);
        ui->detailWidget->setLayout(new QVBoxLayout(ui->detailWidget));

        connect(ui->cbHeaderCompPlugin, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::OnPluginComboBoxSelChanged);
        connect(ui->cbHeaderDepComp, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::OnCompComboBoxSelChanged);

        InitMenu();
    }

    void MainWindow::InitMenu()
    {
    #define INIT_MENU_CONNECT(x, y) connect(x, &QAction::triggered, this, y)

        const Vector<QAction*> action_lst = { ui->actionFileOpen, ui->actionFileClose, ui->actionExportMesh,
                                              ui->actionExportResults, ui->actionViewSetDefaultView,
                                              ui->actionViewSetBackground, ui->actionMeshCreate,
                                              ui->actionSelectRect, ui->actionUnselectRect,
                                              ui->actionViewXY, ui->actionViewYZ, ui->actionViewXZ,
                                              ui->actionViewMirror, ui->actionViewSun, ui->actionViewXYZ,
                                              ui->actionViewWireframe };
        const Vector<decltype(&MainWindow::OnMenuFileOpen)> slot_lst =
            { &MainWindow::OnMenuFileOpen, &MainWindow::OnMenuFileCloseCurrent, &MainWindow::OnMenuFileExportMesh,
              &MainWindow::OnMenuFileExportResults, &MainWindow::OnMenuViewSetDefaultView, &MainWindow::OnMenuViewSetDefaultView,
              &MainWindow::OnMenuMeshCreate, &MainWindow::OnToolSelectRect, &MainWindow::OnToolUnselectRect,
              &MainWindow::OnToolViewXY, &MainWindow::OnToolViewYZ, &MainWindow::OnToolViewXZ,
              &MainWindow::OnToolViewMirror, &MainWindow::OnToolViewSun, &MainWindow::OnToolViewXYZ,
              &MainWindow::OnToolViewWireframe };
        Q_ASSERT(action_lst.size() == slot_lst.size());

        for(int i = 0; i < action_lst.size(); ++i)
            connect(action_lst[i], &QAction::triggered, this, slot_lst[i]);


        ui->projectTree->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->projectTree, &QTreeWidget::itemSelectionChanged,
                this, &MainWindow::OnProjectTreeLClick);
        connect(ui->projectTree, &QTreeWidget::customContextMenuRequested,
                this, &MainWindow::OnProjectTreeRClick);
        connect(ui->projectTree, &QTreeWidget::itemChanged,
                this, &MainWindow::OnTreeItemChanged);

        connect(this, &MainWindow::InitProgressBarSig, this, &MainWindow::InitProgressBar,
                Qt::QueuedConnection);
        connect(this, &MainWindow::SetProgressValueSig, this, &MainWindow::SetProgressValue,
                Qt::QueuedConnection);
        connect(this, &MainWindow::HideProgressBarSig, this, &MainWindow::HideProgressBar,
                Qt::QueuedConnection);

        ui->menubar->setVisible(false);
    }

    void MainWindow::InitEventFollow()
    {
        uint tmp;   // can discard here
        tmp = proj::glev::ProgramStatusChanged.AddFollower(
        [this](const String& msg, const uint timeout)
        {
            ui->statusbar->showMessage(msg, timeout);
        });

        tmp = proj::glev::ProgressBarInitRequested.AddFollower(
        [this](const int min, const int max)
        {
            emit InitProgressBarSig(min, max);
        });
        tmp = proj::glev::ProgressBarChangeValueRequested.AddFollower(
        [this](const int val) { emit SetProgressValueSig(val); });
        tmp = proj::glev::ProgressBarHideRequested.AddFollower(
        [this] { emit HideProgressBarSig(); });

        tmp = proj::glev::GeomEntSelectionEnable.AddFollower(
        [this](const gui::wgt::BaseWidgetGeomSel*,
               const SelectEntityType type)
        {
            if(type == SELECT_ENTITY_TYPE_NONE) return;

            ui->actionSelectRect->setEnabled(true);
            ui->actionUnselectRect->setEnabled(true);

            ui->renderView->GetRenderer()->SetSelectCmd(gui::SELECT_CMD_SINGLE);
        });
        tmp = proj::glev::GeomEntSelectionDisable.AddFollower(
        [this]()
        {
            ui->actionSelectRect->setEnabled(false);
            ui->actionUnselectRect->setEnabled(false);

            ui->renderView->GetRenderer()->SetSelectCmd(gui::SELECT_CMD_NONE);
        });
        tmp = proj::glev::TriangulationFinished.AddFollower([this]{ OnTriangFinished(); });
        tmp = proj::glev::MeshingFinished.AddFollower([this](const MeshingStatus stat) { OnMeshingFinished(stat); });
        tmp = proj::glev::SolvFinished.AddFollower([this](const SolvStatus stat) { OnSolvingFinished(stat); });

        tmp = proj::glev::ActiveComponentBecameDirty.AddFollower([this]{ ActiveCompBecameDirty(); });
    }

    void MainWindow::ChangeStatusMsg(const String& msg,
                                     const uint timeout)
    {
        ui->statusbar->showMessage(msg, timeout);
    }

    void MainWindow::closeEvent(QCloseEvent* ev)
    {
        QMainWindow::closeEvent(ev);

        if(dlg::MboxQuestion("Quit", "Are you sure you want to close "
                             "the application?") == dlg::MboxBtn::No)
        {
            ev->ignore();
            return;
        }

        LockGui();
        SaveSettings();
    }

    void MainWindow::LockGui()
    {
        AddWndState(MAIN_WND_STATE_LOCKED);
    }

    void MainWindow::UnlockGui()
    {
        RemWndState(MAIN_WND_STATE_LOCKED);
    }

    void MainWindow::InitProgressBar(const int min,
                                     const int max)
    {
        progress_bar_->setMinimum(min);
        progress_bar_->setMaximum(max);
        progress_bar_->setValue(min);

        progress_bar_->setVisible(true);
    }

    void MainWindow::SetProgressValue(const int val)
    {
        progress_bar_->setValue(val);
    }

    void MainWindow::HideProgressBar()
    {
        progress_bar_->setVisible(false);
    }

    void MainWindow::OnStartBtnClicked()
    {
        const auto comp_type = active_item_->GetCompType();
        switch (comp_type)
        {
            case core::COMPONENT_TRNG: return StartTriang();
            case core::COMPONENT_MESH: return StartMeshing();
            case core::COMPONENT_SOLV: return StartSolving();
            default: return;
        }
    }

    void MainWindow::OnStopBtnClicked()
    {
        if(!active_item_)
            return;

        switch(active_item_->GetCompType())
        {
        case core::COMPONENT_MESH: proj_->GetMesh()->StopMeshing(); break;
        case core::COMPONENT_SOLV: proj_->GetSolver()->StopSolv(); break;
        //case core::COMPONENT_POST: proj_->GetMesh()->StopMeshing(); break;
        default: break;
        }
        if(curr_par_task_)
            curr_par_task_->SetStopFlag();
    }

    void MainWindow::StartTriang()
    {
        if(!wgt_mgr_->GetTriangInfo(active_item_->GetId(),
                                    *proj_->GetGeometry()->GetStlConvertParams()))
        {
            PrintE("Cannot get polygonalization settings.");
            return;
        }

        if(!proj_->GetGeometry()->StartPolygonize(curr_par_task_))
            return;

        LockGui();
    }

    void MainWindow::StartMeshing()
    {
        if(!wgt_mgr_->GetPluginSettings(core::COMPONENT_PLUGGABLE_MESH, active_item_->GetId(),
                                        *proj_->GetMesh()->GetMesherSettings(active_item_->GetId())))
        {
            PrintE("Cannot get meshing settings for mesher '%1'."_qs.arg(active_item_->text(0)));
            return;
        }
        if(!proj_->GetGeometry()->HasPolygonizedModel())
        {
            PrintE("Unable to start meshing '%1': polygonalization is not produced."_qs.arg(active_item_->text(0)));
            return;
        }

        if(curr_par_task_) delete curr_par_task_;
        curr_par_task_ = new util::ParallelTask(
        [this](const QAtomicInt&)
        {
            if(auto ptr = proj_->GetMesh()->GetCurrentGeometry();
               !ptr || proj_->GetGeometry()->IsDirty())
            {
                if(!proj_->GetGeometry()->ConvertToTriangulation(ptr, ptr))
                {
                    proj_->GetMesh()->SetCurrentGeometry(nullptr);
                    return;
                }

                Print("Conversion of the shape to triangle data has finished. The shape is described by "
                      "%1 vertices and %2 triangles. Starting meshing..."_qs.arg(ptr->NodeLst.size() / 3)
                      .arg(ptr->FaceTriaLst.size()));

                proj_->GetMesh()->SetCurrentGeometry(ptr);
            }
        });
        curr_par_task_->SetOnFinishedFunc(          // executes in the UI thread
        [this]()
        {
            if(!proj_->GetMesh()->GetCurrentGeometry())
            {
                PrintE("Unable to start meshing '%1': triangle form of the shape is not available."_qs
                       .arg(active_item_->text(0)));
                UnlockGui();
                return;
            }

            // have to set this right prior to thread lauch and unset if none happened.
            proj::globj::gCurrentComponent = core::COMPONENT_PLUGGABLE_MESH;

            if(!proj_->GetMesh()->StartMeshing(active_item_->GetId(), curr_par_task_))
            {
                UnlockGui();
                proj::globj::gCurrentComponent = core::COMPONENT_PLUGGABLE_ENUM_COUNT;
                return;
            }
        });

        LockGui();
        curr_par_task_->start();
    }

    void MainWindow::StartSolving()
    {
        if(!wgt_mgr_->GetPluginSettings(core::COMPONENT_PLUGGABLE_SOLV, active_item_->GetId(),
                                        *proj_->GetSolver()->GetSolverSettings(active_item_->GetId())))
        {
            PrintE("Cannot get solving settings for solver '%1'."_qs.arg(active_item_->text(0)));
            return;
        }

        proj_->GetSolver()->SetSolverMesh(active_item_->GetId(),
                                          proj_->GetMesh()->GetMesherRes(active_item_->GetDepId()));
        if(!proj_->GetSolver()->GetSolverMesh(active_item_->GetId()))
        {
            PrintE("Unable to start solving '%1': mesh is not generated."_qs.arg(active_item_->text(0)));
            return;
        }

        // have to set this right prior to thread lauch and unset if none happened.
        proj::globj::gCurrentComponent = core::COMPONENT_PLUGGABLE_SOLV;

        if(!proj_->GetSolver()->StartSolv(active_item_->GetId(), curr_par_task_))
        {
            proj::globj::gCurrentComponent = core::COMPONENT_PLUGGABLE_ENUM_COUNT;
            return;
        }

        LockGui();
    }

    void MainWindow::OnTriangFinished()
    {
        SAFE_DEL(curr_par_task_);

        if(!proj_->GetGeometry()->HasPolygonizedModel())
        {
            UnlockGui();
            return;
        }

        Print("Polygonalization has finished.");

        ui->renderView->GetRenderer()->GetRenderList().
                SetCurrentTrianGeometry(proj_->GetGeometry()->GetPolygonizedGeomVis());

        ui->projectTree->clearSelection();
        active_item_->setSelected(true);
        active_item_->SetReadyIcon(true);

        UnlockGui();
    }

    void MainWindow::OnMeshingFinished(const MeshingStatus stat)
    {
        auto item = active_item_;

        if(stat != MESHING_STATUS_FINISHED)
        {
            if(stat == MESHING_STATUS_ERROR)
            {
                PrintE("Mesher '%1' has failed."_qs.arg(item->text(0)));
            }
            UnlockGui();
            return;
        }

        Print("Mesher '%1' has finished."_qs.arg(item->text(0)));

        ui->renderView->GetRenderer()->GetRenderList().
                SetCurrentMesh(proj_->GetMesh()->GetMeshVis(item->GetId()));

        ui->projectTree->clearSelection();
        item->setSelected(true);
        item->SetReadyIcon(true);

        UnlockGui();
    }

    void MainWindow::OnSolvingFinished(const SolvStatus st)
    {
        auto item = active_item_;

        if(st != SOLV_STATUS_FINISHED)
        {
            if(st == SOLV_STATUS_NOT_STARTED)
            {
                PrintE("Solver '%1' could not start."_qs.arg(item->text(0)));
            }
            if(st == SOLV_STATUS_ERROR)
            {
                PrintE("Solver '%1' has failed."_qs.arg(item->text(0)));
            }
            UnlockGui();
            return;
        }

        Print("Solver '%1' has finished."_qs.arg(item->text(0)));

        TreeWidgetItem* post_item = nullptr;  // find corresp post item
        const auto slpst_num = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT]->childCount();
        for(int i = 0; i < slpst_num; ++i)
            if(auto ch = static_cast<TreeWidgetItem*>(tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT]->child(i));
               ch->GetDepId() == item->GetId())
            {
                post_item = ch;
                break;
            }

        UnlockGui();

        if(!post_item)
            return;

        OnTreePostInstAddNodalSol(post_item, {0, "Total displacement", {} });   // plot deformed shape
    }

    void MainWindow::UnloadGeometry()
    {
        RemWndState(MAIN_WND_STATE_SHOW_SHAPE | MAIN_WND_STATE_SHOW_TRIAN);

        ui->renderView->GetRenderer()->GetRenderList().
                RemoveTrianGeometry(proj_->GetGeometry()->GetPolygonizedGeomVis());
        ui->renderView->GetRenderer()->GetRenderList().
                RemoveGeometry(proj_->GetGeometry()->GetGeomVis());

        proj_->GetGeometry()->Clear();
    }

    void MainWindow::UnloadTriang()
    {
        RemWndState(MAIN_WND_STATE_SHOW_TRIAN);

        ui->renderView->GetRenderer()->GetRenderList().
                RemoveTrianGeometry(proj_->GetGeometry()->GetPolygonizedGeomVis());

        auto parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN];
        for(int i = parent->childCount() - 1; i >= 0; --i)
        {
            auto ch = static_cast<TreeWidgetItem*>(parent->child(i));
            OnTreeGeomTrianInstDelete(ch, false);
        }

        proj_->GetGeometry()->RemovePolygonizedModel();
    }

    void MainWindow::UnloadMesh()
    {
        RemWndState(MAIN_WND_STATE_SHOW_MESH);

        active_inst_lst_[core::COMPONENT_PLUGGABLE_MESH] = nullptr;

        auto parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH];
        for(int i = parent->childCount() - 1; i > 0; --i)
        {
            auto ch = static_cast<TreeWidgetItem*>(parent->child(i));
            OnTreeMeshInstDelete(ch, false);
        }

        proj_->GetMesh()->Clear();
    }

    void MainWindow::UnloadMeshPlugs()
    {
        auto parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH_PLUGIN];
        for(int i = parent->childCount() - 1; i >= 0; --i)
        {
            UnloadPlugin(static_cast<TreeWidgetItem*>(parent->child(i)), false);
        }
    }

    void MainWindow::UnloadSolver()
    {
        active_inst_lst_[core::COMPONENT_PLUGGABLE_SOLV] = nullptr;

        auto parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER];
        for(int i = parent->childCount() - 1; i > 0; --i)
        {
            auto ch = static_cast<TreeWidgetItem*>(parent->child(i));
            OnTreeSolvInstDelete(ch, false);
        }

        proj_->GetSolver()->Clear();
    }

    void MainWindow::UnloadSolverPlugs()
    {
        auto parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN];
        for(int i = parent->childCount() - 1; i >= 0; --i)
        {
            UnloadPlugin(static_cast<TreeWidgetItem*>(parent->child(i)), false);
        }
    }

    void MainWindow::ShowActiveMesher()
    {
        auto item = active_inst_lst_[core::COMPONENT_PLUGGABLE_MESH];

        RefillPluginComboBox(core::COMPONENT_PLUGGABLE_MESH);

        active_item_ = nullptr;     // to forbide change of settings
            ui->cbHeaderCompPlugin->setCurrentIndex(ui->cbHeaderCompPlugin->findData(item->GetPluginId()));
        active_item_ = item;

        RemWndState(MAIN_WND_STATE_SHOW_ENUMMASK);
        if(!(wnd_state_ & MAIN_WND_STATE_SHOW_MESH))
        {
            AddWndState(proj_->GetMesh()->HasMeshVis(item->GetId()) ?
                            MAIN_WND_STATE_SHOW_MESH : MAIN_WND_STATE_SHOW_SHAPE);
        }

        wgt_mgr_->SetHeaderStyle(core::COMPONENT_MESH, item->text(0));
        wgt_mgr_->ShowWidgetCont(core::COMPONENT_PLUGGABLE_MESH, item->GetId());
    }

    void MainWindow::ShowActiveSolver()
    {
        auto item = active_inst_lst_[core::COMPONENT_PLUGGABLE_SOLV];

        RefillPluginComboBox(core::COMPONENT_PLUGGABLE_SOLV);
        RefillCompComboBox(core::COMPONENT_PLUGGABLE_SOLV);

        active_item_ = nullptr;     // to forbide change of settings
            ui->cbHeaderCompPlugin->setCurrentIndex(ui->cbHeaderCompPlugin->findData(item->GetPluginId()));
            ui->cbHeaderDepComp->setCurrentIndex(ui->cbHeaderDepComp->findData(item->GetDepId()));
        active_item_ = item;

        RemWndState(MAIN_WND_STATE_SHOW_ENUMMASK);
        if(!(wnd_state_ & MAIN_WND_STATE_SHOW_SHAPE))
        {
            AddWndState(MAIN_WND_STATE_SHOW_SHAPE);
        }

        wgt_mgr_->SetHeaderStyle(core::COMPONENT_SOLV, item->text(0));
        wgt_mgr_->ShowWidgetCont(core::COMPONENT_PLUGGABLE_SOLV, item->GetId());
    }

    void MainWindow::ShowActivePostpr()
    {
        auto item = active_inst_lst_[core::COMPONENT_PLUGGABLE_POST];
        const auto sid = item->GetDepId();
        const auto dsid = item->GetPluginId();      // here it's dataset_id

        auto sv = proj_->GetPostpr()->GetSolVis(sid, dsid);
        ui->renderView->GetRenderer()->GetRenderList().SetCurrentNodalSol(sid, dsid, sv);

        this->RemWndState(MAIN_WND_STATE_SHOW_ENUMMASK);
        if(sv)
            this->AddWndState(MAIN_WND_STATE_SHOW_SOLU);

        wgt_mgr_->SetHeaderStyle(core::COMPONENT_POST, item->text(0));
        wgt_mgr_->HideWidget();
    }

    void MainWindow::LoadPlugin(const core::ComponentPluggable comp)
    {
        String dll_file;
        if(!dlg::OpenFileDlg("Choose plugin file...", "Dynamic library (*.dll)", dll_file))
           return;

        plug::plugid_t pid = plug::PLUGIN_ID_NULL;
        const plug::PluginInfo* pinfo = nullptr;
        switch (comp)
        {
            case core::COMPONENT_PLUGGABLE_MESH: pid = proj_->GetMesh()->LoadPlugin(dll_file); break;
            case core::COMPONENT_PLUGGABLE_SOLV: pid = proj_->GetSolver()->LoadPlugin(dll_file); break;
            default: break;
        }

        if(pid == plug::PLUGIN_ID_NULL)
        {
            PrintE("Error: cannot load plugin '%1'."_qs.arg(dll_file));
            return;
        }

        QTreeWidgetItem* parent = nullptr;
        TreeWidgetItemType type = TREE_WIDGET_ITEM_TYPE_MESH;   // as default
        if(comp == core::COMPONENT_PLUGGABLE_MESH)
        {
            pinfo = proj_->GetMesh()->GetPluginInfo(pid);
            parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH_PLUGIN];
            type = TREE_WIDGET_ITEM_TYPE_MESH;
        }
        else
        if(comp == core::COMPONENT_PLUGGABLE_SOLV)
        {
            pinfo = proj_->GetSolver()->GetPluginInfo(pid);
            parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN];
            type = TREE_WIDGET_ITEM_TYPE_SOLV;
        }

        if(!parent) return;

        auto itm = new TreeWidgetItem(parent, type, pid);
        itm->setFlags(itm->flags() | Qt::ItemIsEditable);
        itm->setText(0, GenerateItemName(itm, pinfo->Name));
        itm->SetReadyIcon(true);
        parent->setExpanded(true);

        if(comp == core::COMPONENT_PLUGGABLE_MESH)
        {
            wgt_mgr_->AddWidgetPlug(itm->GetId(), pinfo->ToString());
        }
        else
        if(comp == core::COMPONENT_PLUGGABLE_SOLV)
        {
            wgt_mgr_->AddWidgetPlug(itm->GetId(), pinfo->ToString());
        }

        ui->projectTree->clearSelection();
        itm->setSelected(true);
    }

    void MainWindow::UnloadPlugin(TreeWidgetItem* item, bool by_user)
    {
        if(by_user)
        if(dlg::MboxQuestion("Unload plugin...",
                             "Are you sure you want to unload '%1'?"_qs
                             .arg(item->text(0))) == dlg::MboxBtn::No)
        {
            return;
        }

        LockGui();

        wgt_mgr_->RemoveWidgetPlug(item->GetId());

        const auto pid = item->GetPluginId();
        const auto ptype = item->GetCompType();

        switch (ptype)
        {
        case core::COMPONENT_MESH:
            proj_->GetMesh()->UnloadPlugin(pid);
            UpdatePluggableInstances(core::COMPONENT_PLUGGABLE_MESH, pid);
            break;
        case core::COMPONENT_SOLV:
            proj_->GetSolver()->UnloadPlugin(pid);
            UpdatePluggableInstances(core::COMPONENT_PLUGGABLE_SOLV, pid);
            break;
//        case core::COMPONENT_POST: proj_->GetMesh()->UnloadPlugin(pid); break;
        default: UnlockGui(); return;
        }

        item->parent()->removeChild(item);
        delete item;

        UnlockGui();
    }

    void MainWindow::UpdatePluggableInstances(const core::ComponentPluggable comp,
                                              const plug::plugid_t pid)
    {
        QTreeWidgetItem* parent;
        switch(comp)
        {
        case core::COMPONENT_PLUGGABLE_MESH: parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH]; break;
        case core::COMPONENT_PLUGGABLE_SOLV: parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER]; break;
        case core::COMPONENT_PLUGGABLE_POST: parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT]; break;
        default: return;
        }

        for(int i = 1; i < parent->childCount(); ++i)   // 0th is plugin tree
            if(auto ch = static_cast<TreeWidgetItem*>(parent->child(i));
               ch->GetPluginId() == pid)
            {
                plug::PluginSettings* sets = nullptr;
                if(comp == core::COMPONENT_PLUGGABLE_MESH)
                {
                    bool was_current;
                    if(ui->renderView->GetRenderer()->GetRenderList().
                       RemoveMesh(proj_->GetMesh()->GetMeshVis(ch->GetId()), &was_current)
                       && was_current
                    )
                       RemWndState(MAIN_WND_STATE_SHOW_MESH);

                    proj_->GetMesh()->ClearMeshResults(ch->GetId());
                    sets = proj_->GetMesh()->GetMesherSettings(ch->GetId());
                }
                else
                if(comp == core::COMPONENT_PLUGGABLE_SOLV)
                {
                    proj_->GetSolver()->ClearSolvResults(ch->GetId());
                    sets = proj_->GetSolver()->GetSolverSettings(ch->GetId());
                }

                ch->SetPluginId(plug::PLUGIN_ID_NULL);
                wgt_mgr_->ChangeWidgetContSets(comp, ch->GetId(), *sets);
            }
    }

    void MainWindow::ActiveCompBecameDirty()
    {
        active_item_->SetReadyIcon(false);
    }

    void MainWindow::RefillPluginComboBox(const core::ComponentPluggable comp)
    {
        QTreeWidgetItem* parent;
        switch (comp)
        {
        case core::COMPONENT_PLUGGABLE_MESH: parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH_PLUGIN]; break;
        case core::COMPONENT_PLUGGABLE_SOLV: parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN]; break;
        default: return;
        }

        {
            QSignalBlocker bl(ui->cbHeaderCompPlugin);

            ui->cbHeaderCompPlugin->clear();
            ui->cbHeaderCompPlugin->addItem("None", 0);
        }

        if(parent->childCount() < 1)
            return;

        for(int i = 0; i < parent->childCount(); ++i)
        {
            auto pl = static_cast<TreeWidgetItem*>(parent->child(i));
            ui->cbHeaderCompPlugin->addItem(pl->text(0), pl->GetPluginId());
        }
    }

    void MainWindow::RefillCompComboBox(const core::ComponentPluggable comp)
    {
        QTreeWidgetItem* parent;
        switch (comp)
        {
        case core::COMPONENT_PLUGGABLE_SOLV: parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH]; break;
        case core::COMPONENT_PLUGGABLE_POST: parent = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER]; break;
        default: return;
        }

        {
            QSignalBlocker bl(ui->cbHeaderDepComp);

            ui->cbHeaderDepComp->clear();
            ui->cbHeaderDepComp->addItem("None", 0);
        }

        if(parent->childCount() < 2)
            return;

        for(int i = 1; i < parent->childCount(); ++i)
        {
            auto cmp = static_cast<TreeWidgetItem*>(parent->child(i));
            ui->cbHeaderDepComp->addItem(cmp->text(0), cmp->GetId());
        }
    }

    void MainWindow::OnPluginComboBoxSelChanged(int idx)
    {
        if(!active_item_)
            return;

        active_item_->SetPluginId(ui->cbHeaderCompPlugin->itemData(idx).toInt());

        const auto type = active_item_->GetCompType();
        core::ComponentPluggable pl_type;
        int comp_id = active_item_->GetId();
        if(type == core::COMPONENT_MESH)
        {
            ActiveCompBecameDirty();

            proj_->GetMesh()->SetMesherPlugin(comp_id, active_item_->GetPluginId());
            if(!wgt_mgr_->ChangeWidgetContSets(pl_type = core::COMPONENT_PLUGGABLE_MESH,
                                               comp_id, *proj_->GetMesh()->GetMesherSettings(comp_id)))
            {
                ui->cbHeaderCompPlugin->setCurrentIndex(0); // null
                return;
            }
        }
        else
        if(type == core::COMPONENT_SOLV)
        {
            ActiveCompBecameDirty();

            proj_->GetSolver()->SetSolverPlugin(comp_id, active_item_->GetPluginId());
            if(!wgt_mgr_->ChangeWidgetContSets(pl_type = core::COMPONENT_PLUGGABLE_SOLV,
                                               comp_id, *proj_->GetSolver()->GetSolverSettings(comp_id)))
            {
                ui->cbHeaderCompPlugin->setCurrentIndex(0); // null
                return;
            }
        }
        else
        if(type == core::COMPONENT_POST)
        {
            proj_->GetMesh()->SetMesherPlugin(comp_id, active_item_->GetPluginId());
            wgt_mgr_->ChangeWidgetContSets(pl_type = core::COMPONENT_PLUGGABLE_MESH,
                                           comp_id,
                                           *proj_->GetMesh()->GetMesherSettings(comp_id));
        }
        else
            return;

        wgt_mgr_->ShowWidgetCont(pl_type, comp_id);
    }

    void MainWindow::OnCompComboBoxSelChanged(int idx)
    {
        if(!active_item_)
            return;

        const auto type = active_item_->GetCompType();
        if(type == core::COMPONENT_SOLV)
        {
            ActiveCompBecameDirty();

            active_item_->SetDep(ui->cbHeaderDepComp->itemData(idx).toInt(), TREE_WIDGET_ITEM_TYPE_MESH);
        }
        else
        if(type == core::COMPONENT_POST)
        {
            //proj_->GetMesh()->SetMesherPlugin(comp_id, active_item_->GetPluginId());
        }
        else
            return;
    }

    void MainWindow::OnProjectTreeLClick()
    {
        const auto sel_lst = ui->projectTree->selectedItems();
        if(sel_lst.empty())
            return;

        active_item_ = nullptr;

        auto item = sel_lst.first();

        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_ROOT])
        {
            wgt_mgr_->HideHeader();
            wgt_mgr_->HideWidget();
        }
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM])
        {
            RemWndState(MAIN_WND_STATE_SHOW_ENUMMASK);
            if(!(wnd_state_ & MAIN_WND_STATE_SHOW_SHAPE) &&
               proj_->GetGeometry()->HasModel())
            {
                AddWndState(MAIN_WND_STATE_SHOW_SHAPE);
            }

            wgt_mgr_->SetHeaderStyle(core::COMPONENT_GEOM, "");
            wgt_mgr_->HideWidget();
        }
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN] ||
           item->parent() == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN])
        {
            auto ch = static_cast<TreeWidgetItem*>
                      (tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN]->child(0));
            if(!ch)
            {
                wgt_mgr_->HideHeader();
                wgt_mgr_->HideWidget();
                return;
            }

            active_item_ = ch;

            RemWndState(MAIN_WND_STATE_SHOW_ENUMMASK);
            if(!(wnd_state_ & MAIN_WND_STATE_SHOW_TRIAN) &&
               proj_->GetGeometry()->HasPolygonizedModel())
            {
                AddWndState(MAIN_WND_STATE_SHOW_TRIAN);
            }

            wgt_mgr_->SetHeaderStyle(core::COMPONENT_TRNG, ch->text(0));
            wgt_mgr_->ShowWidgetTriang(ch->GetId());
        }
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH])
        {
            auto act_inst = active_inst_lst_[core::COMPONENT_PLUGGABLE_MESH];
            if(!act_inst)
            {
                wgt_mgr_->HideHeader();
                wgt_mgr_->HideWidget();
                return;
            }

            ShowActiveMesher();
        }
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER])
        {
            auto act_inst = active_inst_lst_[core::COMPONENT_PLUGGABLE_SOLV];
            if(!act_inst)
            {
                wgt_mgr_->HideHeader();
                wgt_mgr_->HideWidget();
                return;
            }

            ShowActiveSolver();
        }
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT])
        {
            auto act_inst = active_inst_lst_[core::COMPONENT_PLUGGABLE_POST];
            if(!act_inst)
            {
                wgt_mgr_->HideHeader();
                wgt_mgr_->HideWidget();
                return;
            }
        }
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH_PLUGIN] ||
           item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN])
        {
            wgt_mgr_->HideHeader();
            wgt_mgr_->HideWidget();
        }
        else
        if(item->parent() == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH])
        {
            auto cast = static_cast<TreeWidgetItem*>(item);
            if(!cast)
                return;

            active_item_ = active_inst_lst_[core::COMPONENT_PLUGGABLE_MESH] = cast;

            ShowActiveMesher();
        }
        else
        if(item->parent() == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER])
        {
            auto cast = static_cast<TreeWidgetItem*>(item);
            if(!cast)
                return;

            active_item_ = active_inst_lst_[core::COMPONENT_PLUGGABLE_SOLV] = cast;

            ShowActiveSolver();
        }
        else
        if(item->parent() && item->parent()->parent() == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT])  // some postpr
        {
            auto cast = static_cast<TreeWidgetItem*>(item);
            if(!cast)
                return;

            active_item_ = active_inst_lst_[core::COMPONENT_PLUGGABLE_POST] = cast;

            ShowActivePostpr();
        }
        else
        if(item->parent() == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH_PLUGIN] ||
           item->parent() == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN])
        {
            auto cast = static_cast<TreeWidgetItem*>(item);
            if(!cast)
            {
                wgt_mgr_->HideHeader();
                wgt_mgr_->HideWidget();
                return;
            }

            wgt_mgr_->HideHeader();
            wgt_mgr_->ShowWidgetPlug((active_item_ = cast)->GetId());
        }
        else
        {}
    }

    void MainWindow::OnProjectTreeRClick(const QPoint& pnt)
    {
        const auto tree = ui->projectTree;
        auto item = tree->itemAt(pnt);
        if(!item)
            return;

        QMenu menu(this);

        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_ROOT])
            FillTreeItemContextMenuRoot(menu);
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM])
            FillTreeItemContextMenuGeom(menu);
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN])
            FillTreeItemContextMenuGeomTrian(menu);
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH])
            FillTreeItemContextMenuMesh(menu);
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH_PLUGIN])
            FillTreeItemContextMenuPlugin(menu, core::COMPONENT_PLUGGABLE_MESH);
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER])
            FillTreeItemContextMenuSolv(menu);
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN])
            FillTreeItemContextMenuPlugin(menu, core::COMPONENT_PLUGGABLE_SOLV);
        else
        if(item == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT])
            FillTreeItemContextMenuPost(menu);
        else
        {
            // next possible variants are already created components.
            // They are allowed e.g. to be renamed

            const auto parent = item->parent();
            auto cast = static_cast<TreeWidgetItem*>(item);

//            if(parent == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM])
//                FillTreeItemContextMenuGeomComp(menu, cast);
//            else
            if(parent == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN])
                FillTreeItemContextMenuGeomTrianInst(menu, cast);
            else
            if(parent == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH]) // that's mesher instance
                FillTreeItemContextMenuMeshInst(menu, cast);
            else
            if(parent == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER])
                FillTreeItemContextMenuSolvInst(menu, cast);
            else
            if(parent == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT])
                FillTreeItemContextMenuPostInst(menu, cast);
            else
            if(parent == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH_PLUGIN] ||
               parent == tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER_PLUGIN])
                FillTreeItemContextMenuPluginInst(menu, cast);
            else
                return;

            menu.addSeparator();
            FillTreeItemContextMenuGenericComp(menu, cast);
        }


        menu.exec(tree->mapToGlobal(pnt));
    }

    void MainWindow::OnTreeProjUnload()
    {
        if(QMessageBox::question(this, "Unload project", "Are you sure you want to unload the project?")
           == QMessageBox::No)
            return;

        LockGui();
        RemWndState(MAIN_WND_STATE_SHOW_ENUMMASK);

        UnloadSolverPlugs();
        UnloadSolver();
        UnloadMeshPlugs();
        UnloadMesh();
        UnloadTriang();
        UnloadGeometry();

        UnlockGui();
    }

    void MainWindow::OnTreeGeomLoad()
    {
        String file_name;
        if(!dlg::OpenFileDlg("Choose geometry file...",
                             "STEP format (*.step *.stp *.p21);;"
                             "STL format (*.stl);;"
                             "All files (*.*)",
                             file_name))
            return;

        auto load_task = new util::ParallelTask(
        [this, file_name](const QAtomicInt&)
        {
             proj_->GetGeometry()->Load(file_name);
        });
        connect(load_task, &util::ParallelTask::finished,
                load_task, &util::ParallelTask::deleteLater);
        load_task->SetOnFinishedFunc(
        [this]
        {
            ui->renderView->GetRenderer()->GetRenderList().
                    SetCurrentGeometry(proj_->GetGeometry()->GetGeomVis());

            RemWndState(MAIN_WND_STATE_SHOW_ENUMMASK);
            AddWndState(MAIN_WND_STATE_SHOW_SHAPE);
            OnToolViewXYZ();

            tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM]->setExpanded(true);

            Print("Geometry has been loaded.");

            HideProgressBar();
            UnlockGui();
        });

        LockGui();
        InitProgressBar(0, 0);
        load_task->start();
    }

    void MainWindow::OnTreeGeomTrianCreate()
    {
        if(!proj_->GetGeometry()->HasModel())
        {
            PrintE("Unable to create polygonalization: no geometry was loaded.");
            return;
        }
        if(tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN]->childCount())
        {
            PrintE("Unable to create polygonalization: one already exists.");
            return;
        }

        auto itm = new TreeWidgetItem(tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN],
                                      TREE_WIDGET_ITEM_TYPE_TRNG);
        wgt_mgr_->AddWidgetTriang(itm->GetId());

        itm->setFlags(itm->flags() | Qt::ItemIsEditable);
        tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_GEOM_TRIAN]->setExpanded(true);
        tree_item_name_before_edit_ = itm->text(0);
        itm->setText(0, GenerateItemName(itm, itm->GetTreeItemTypePrefixName(itm->GetType())));
    }

    void MainWindow::OnTreeGeomTrianInstDelete(TreeWidgetItem* item, const bool by_user)
    {
        if(by_user)
        if(dlg::MboxBtn::No == dlg::MboxQuestion("Delete polygonalization...",
                                                 "Are you sure you want to delete '%1'?"_qs
                                                 .arg(item->text(0))))
        {
            return;
        }

        LockGui();

        wgt_mgr_->RemoveWidgetTriang(item->GetId());
        item->parent()->removeChild(item);
        delete item;

        UnlockGui();
    }

    void MainWindow::OnTreeMeshInstCreate()
    {
        if(!proj_->GetGeometry()->HasModel())
        {
            PrintE("Unable to create mesh: no geometry was loaded.");
            return;
        }

        auto itm = new TreeWidgetItem(tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH],
                                      TREE_WIDGET_ITEM_TYPE_MESH);
        proj_->GetMesh()->CreateMesher(itm->GetId());
        wgt_mgr_->AddWidgetCont(core::COMPONENT_PLUGGABLE_MESH, itm->GetId());

        itm->setFlags(itm->flags() | Qt::ItemIsEditable);
        tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_MESH]->setExpanded(true);
        itm->setText(0, GenerateItemName(itm, itm->GetTreeItemTypePrefixName(itm->GetType())));

        ui->projectTree->clearSelection();
        itm->setSelected(true);
    }

    void MainWindow::OnTreeMeshInstDelete(TreeWidgetItem* item, const bool by_user)
    {
        if(by_user)
        if(dlg::MboxQuestion("Delete mesh...",
                             "Are you sure you want to delete '%1'?"_qs
                             .arg(item->text(0))) == dlg::MboxBtn::No)
        {
            return;
        }

        LockGui();

        RemWndState(MAIN_WND_STATE_SHOW_MESH);

        ui->renderView->GetRenderer()->GetRenderList().
                RemoveMesh(proj_->GetMesh()->GetMeshVis(item->GetId()));

        wgt_mgr_->RemoveWidgetCont(core::COMPONENT_PLUGGABLE_MESH, item->GetId());
        proj_->GetMesh()->RemoveMesher(item->GetId());

        item->parent()->removeChild(item);
        delete item;

        active_item_ = active_inst_lst_[core::COMPONENT_PLUGGABLE_MESH] = nullptr;

        UnlockGui();
    }

    void MainWindow::OnTreeMeshInstPrintStats(TreeWidgetItem* item)
    {
        String stats;
        if(!proj_->GetMesh()->GetMeshStatistics(item->GetId(), stats))
            return;

        Print("\t\t\tMESH '%1' STATISTICS\n"_qs.arg(item->text(0)) + stats);
    }

    void MainWindow::OnTreeSolvInstCreate()
    {
        if(!proj_->GetGeometry()->HasModel())
        {
            PrintE("Unable to create solver: no geometry was loaded.");
            return;
        }

        auto itm = new TreeWidgetItem(tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER],
                                      TREE_WIDGET_ITEM_TYPE_SOLV);
        proj_->GetSolver()->CreateSolver(itm->GetId());
        wgt_mgr_->AddWidgetCont(core::COMPONENT_PLUGGABLE_SOLV, itm->GetId());

        tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_SOLVER]->setExpanded(true);
        itm->setFlags(itm->flags() | Qt::ItemIsEditable);
        itm->setText(0, GenerateItemName(itm, itm->GetTreeItemTypePrefixName(itm->GetType())));

        auto itm2 = new TreeWidgetItem(tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT],
                                       TREE_WIDGET_ITEM_TYPE_POST);
        tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT]->setExpanded(true);
        itm2->setFlags(itm2->flags() & ~Qt::ItemIsEditable);
        itm2->setText(0, itm->text(0));
        itm2->SetDep(itm->GetId(), TREE_WIDGET_ITEM_TYPE_SOLV);

        ui->projectTree->clearSelection();
        itm->setSelected(true);
    }

    void MainWindow::OnTreeSolvInstDelete(TreeWidgetItem* item, const bool by_user)
    {
        if(by_user)
        if(dlg::MboxQuestion("Delete solver...",
                             "Are you sure you want to delete '%1'?"_qs
                             .arg(item->text(0))) == dlg::MboxBtn::No)
        {
            return;
        }

        LockGui();

        RemWndState(MAIN_WND_STATE_SHOW_SHAPE);

        // remove all results from this solver
        proj_->GetPostpr()->RemAllSol(item->GetId());

        wgt_mgr_->RemoveWidgetCont(core::COMPONENT_PLUGGABLE_SOLV, item->GetId());
        proj_->GetSolver()->RemoveSolver(item->GetId());

        TreeWidgetItem* post_item = nullptr;  // find corresp post item
        const auto slpst_num = tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT]->childCount();
        for(int i = 0; i < slpst_num; ++i)
            if(auto ch = static_cast<TreeWidgetItem*>(tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT]->child(i));
               ch->GetDepId() == item->GetId())
            {
                post_item = ch;
                break;
            }
        if(post_item)
        {
            tree_main_item_lst_[PROJ_TREE_MAIN_ITEM_RESULT]->removeChild(post_item);
            delete post_item;
        }

        item->parent()->removeChild(item);
        delete item;

        active_item_ = active_inst_lst_[core::COMPONENT_PLUGGABLE_SOLV] = nullptr;

        UnlockGui();
    }
    void MainWindow::OnTreeSolvInstPrintStats(TreeWidgetItem* item)
    {
        String stats;
        if(!proj_->GetSolver()->GetSolvStatistics(item->GetId(), stats))
            return;

        Print("\t\t\tSOLUTION '%1' STATISTICS\n"_qs.arg(item->text(0)) + stats);
    }

    void MainWindow::OnTreePostInstAddNodalSol(TreeWidgetItem* item, const post::DataSetInfo& ds)
    {
        const auto dsid = ds.Id;
        const auto sid = item->GetDepId();

        // ! using dsid as plug_id!
        TreeWidgetItem* post_item = nullptr;
        const auto post_num = item->childCount();
        for(int i = 0; i < post_num; ++i)
            if(auto ch = static_cast<TreeWidgetItem*>(item->child(i));
               ch->GetPluginId() == dsid)
            {
                post_item = ch;
                break;
            }
        if(!post_item)
        {
            post_item = new TreeWidgetItem(item, TREE_WIDGET_ITEM_TYPE_POST, dsid);
            post_item->SetDep(sid, TREE_WIDGET_ITEM_TYPE_SOLV);
            post_item->setText(0, ds.Name);
        }

        LockGui();

        if(curr_par_task_) delete curr_par_task_;
        curr_par_task_ = new util::ParallelTask(
        [this, sid, dsid](const QAtomicInt&)
        {
            auto ns = new post::NodalSol();
            if(!proj_->GetSolver()->GetNodalSol(sid, dsid, *ns))
            {
                UnlockGui();
                return;
            }

            proj_->GetPostpr()->AddNsol(sid, dsid, ns);

        });
        curr_par_task_->SetOnFinishedFunc(
        [this, post_item]()
        {
            active_item_ = active_inst_lst_[core::COMPONENT_PLUGGABLE_POST] = post_item;
            ui->projectTree->clearSelection();
            post_item->setSelected(true);

            UnlockGui();
        });

        curr_par_task_->start();
    }

    void MainWindow::OnTreeItemChanged(QTreeWidgetItem* item, const int col)
    {
        if(tree_item_name_before_edit_.isEmpty())
            return;

        const auto cast = dynamic_cast<const TreeWidgetItem*>(item);
        if(!cast)
            return;

        const auto txt = cast->text(col);
        if(tree_item_name_before_edit_ == txt)
            return;

        bool exists;
        CountSimiliarNames(cast, cast->text(col), &exists);
        if(exists)
        {
            gui::dlg::MboxCritical("Error", "A component with such name already exists.");
            ui->projectTree->editItem(item, col);
            return;
        }

        tree_item_name_before_edit_ = "";

        OnProjectTreeLClick();
    }

    void MainWindow::OnMenuFileOpen()
    {
//        String file;
//        if(!gui::dlg::OpenFileDlg(file))
//            return;
    }
    void MainWindow::OnMenuFileCloseCurrent()
    {
    }

    void MainWindow::OnMenuFileExportMesh()
    {

    }

    void MainWindow::OnMenuFileExportResults()
    {

    }

    void MainWindow::OnMenuViewSetDefaultView()
    {
        return;
    }

    void MainWindow::OnMenuViewBackground()
    {

    }

    void MainWindow::OnMenuMeshCreate()
    {

    }

    void MainWindow::OnToolSelectRect()
    {
        if(ui->actionSelectRect->isChecked())
        {
            ui->renderView->GetRenderer()->SetSelectCmd(gui::SELECT_CMD_RECT_ADD);
            ui->actionUnselectRect->setChecked(false);
        }
        else
            ui->renderView->GetRenderer()->SetSelectCmd(gui::SELECT_CMD_SINGLE);
    }

    void MainWindow::OnToolUnselectRect()
    {
        if(ui->actionUnselectRect->isChecked())
        {
            ui->renderView->GetRenderer()->SetSelectCmd(gui::SELECT_CMD_RECT_REM);
            ui->actionSelectRect->setChecked(false);
        }
        else
            ui->renderView->GetRenderer()->SetSelectCmd(gui::SELECT_CMD_SINGLE);
    }

    void MainWindow::OnToolViewXYZ()
    {
        ui->renderView->GetRenderer()->SetCameraView(cmd::CAMERA_VIEW_XYZ);
    }

    void MainWindow::OnToolViewXY()
    {
        ui->renderView->GetRenderer()->SetCameraView(cmd::CAMERA_VIEW_XY);
    }

    void MainWindow::OnToolViewYZ()
    {
        ui->renderView->GetRenderer()->SetCameraView(cmd::CAMERA_VIEW_YZ);
    }

    void MainWindow::OnToolViewXZ()
    {
        ui->renderView->GetRenderer()->SetCameraView(cmd::CAMERA_VIEW_XZ);
    }

    void MainWindow::OnToolViewMirror()
    {
        ui->renderView->GetRenderer()->MirrorView();
    }

    void MainWindow::OnToolViewSun()
    {
        ui->renderView->GetRenderer()->SwitchLight(ui->actionViewSun->isChecked());
    }

    void MainWindow::OnToolViewWireframe()
    {
        ui->renderView->GetRenderer()->SetViewMode(ui->actionViewWireframe->isChecked() ?
                                                       cmd::VIEW_MODE_WIREFRAME : cmd::VIEW_MODE_SOLID);
    }

    uint MainWindow::CountSimiliarNames(const TreeWidgetItem* item,
                                        const String& name,
                                        bool* has_exact,
                                        uint* max_avail_num) const
    {
        const auto parent = item->parent();
        if(!parent)
            return 0;

        if(has_exact) *has_exact = false;
        if(max_avail_num) *max_avail_num = 0;

        uint count = 0;
        String tmp_txt;
        for(int i = 0; i < parent->childCount(); ++i)
        {
            const auto itm = parent->child(i);
            if(itm != item && (tmp_txt = itm->text(0)).startsWith(name))
            {
                ++count;
                if(tmp_txt.size() == name.size())
                {
                    if(has_exact && !*has_exact)
                        *has_exact = true;
                }
                if(max_avail_num)
                {
                    bool is_int = false;
                    const auto num = tmp_txt.split(" ").last().toUInt(&is_int);
                    if(is_int && *max_avail_num < num)
                        *max_avail_num = num;
                }
            }
        }

        return count;
    }

    // just appends to base_name a number
    String MainWindow::GenerateItemName(const TreeWidgetItem* item,
                                        const String& base_name) const
    {
        uint max_num;
        CountSimiliarNames(item, base_name, nullptr, &max_num);

        return base_name + " %1"_qs.arg(1 + max_num);
    }

    void MainWindow::FillTreeItemContextMenuRoot(QMenu& menu)
    {
        auto act_rem = new QAction("Unload", this);

        connect(act_rem, &QAction::triggered, this, &MainWindow::OnTreeProjUnload);

        menu.addAction(act_rem);
    }

    void MainWindow::FillTreeItemContextMenuGeom(QMenu& menu)
    {
        auto act_load = new QAction("Load...", this);

        connect(act_load, &QAction::triggered, this, &MainWindow::OnTreeGeomLoad);

        menu.addAction(act_load);
    }

//    void MainWindow::FillTreeItemContextMenuGeomComp(QMenu& menu, TreeWidgetItem* item)
//    {
//        UNUSED_PARAM(menu);
//        UNUSED_PARAM(item);
//    }

    void MainWindow::FillTreeItemContextMenuGeomTrian(QMenu& menu)
    {
        auto act_load = new QAction("Create...", this);

        connect(act_load, &QAction::triggered, this, &MainWindow::OnTreeGeomTrianCreate);

        menu.addAction(act_load);
    }

    void MainWindow::FillTreeItemContextMenuGeomTrianInst(QMenu& menu, TreeWidgetItem* item)
    {
        auto act_rem = new QAction("Delete", this);

        connect(act_rem, &QAction::triggered, this,
        [this, item] { return OnTreeGeomTrianInstDelete(item); });

        menu.addAction(act_rem);
    }

    void MainWindow::FillTreeItemContextMenuMesh(QMenu& menu)
    {
        auto act_create = new QAction("Create...", this);

        connect(act_create, &QAction::triggered, this, &MainWindow::OnTreeMeshInstCreate);

        menu.addAction(act_create);
    }

    void MainWindow::FillTreeItemContextMenuMeshInst(QMenu& menu, TreeWidgetItem* item)
    {
        auto act_delete = new QAction("Delete", this);
        auto act_stats = new QAction("Print stats", this);

        connect(act_delete, &QAction::triggered, this, [this, item] { OnTreeMeshInstDelete(item); });
        connect(act_stats, &QAction::triggered, this, [this, item] { OnTreeMeshInstPrintStats(item); });

        menu.addAction(act_delete);
        menu.addSeparator();
        menu.addAction(act_stats);
    }

    void MainWindow::FillTreeItemContextMenuSolv(QMenu& menu)
    {
        auto act_create = new QAction("Create...", this);

        connect(act_create, &QAction::triggered, this, &MainWindow::OnTreeSolvInstCreate);

        menu.addAction(act_create);
    }

    void MainWindow::FillTreeItemContextMenuSolvInst(QMenu& menu, TreeWidgetItem* item)
    {
        auto act_delete = new QAction("Delete", this);
        auto act_stats = new QAction("Print stats", this);

        connect(act_delete, &QAction::triggered, this, [this, item] { OnTreeSolvInstDelete(item); });
        connect(act_stats, &QAction::triggered, this, [this, item] { OnTreeSolvInstPrintStats(item); });

        menu.addAction(act_delete);
        menu.addSeparator();
        menu.addAction(act_stats);
    }

    void MainWindow::FillTreeItemContextMenuPost(QMenu& menu)
    {
        UNUSED_PARAM(menu);
    }

    void MainWindow::FillTreeItemContextMenuPostInst(QMenu& menu, TreeWidgetItem* item)
    {
        const List<post::DataSetInfo>* ds_lst;
        if(!proj_->GetSolver()->GetPluginDataSetLst(item->GetDepId(), ds_lst))
        {
            PrintE("Can't get dataset list.");
            return;
        }

        // for deformed separately
        auto act_plot = new QAction("Plot total displacement", this);
        connect(act_plot, &QAction::triggered, this, [this, item] { OnTreePostInstAddNodalSol(item, {0, "Total displacement", {} }); });
        menu.addAction(act_plot);

        for(const auto& ds: *ds_lst)
        {
            auto act_plot = new QAction("Plot '%1'"_qs.arg(ds.Name), this);
            connect(act_plot, &QAction::triggered, this, [this, item, &ds] { OnTreePostInstAddNodalSol(item, ds); });

            menu.addAction(act_plot);
        }
    }

    void MainWindow::FillTreeItemContextMenuPlugin(QMenu& menu, const core::ComponentPluggable comp)
    {
        auto act_add = new QAction("New...", this);

        connect(act_add, &QAction::triggered, this, [this, comp] { LoadPlugin(comp); });

        menu.addAction(act_add);
    }

    void MainWindow::FillTreeItemContextMenuPluginInst(QMenu& menu, TreeWidgetItem* item)
    {
        auto act_unl = new QAction("Unload", this);

        connect(act_unl, &QAction::triggered, this,
        [this, item] { UnloadPlugin(item); });

        menu.addAction(act_unl);
    }

    void MainWindow::FillTreeItemContextMenuGenericComp(QMenu& menu, QTreeWidgetItem* item)
    {
        auto act_rename = new QAction("Rename");

        connect(act_rename, &QAction::triggered, this,
        [this, item]()
        {
            tree_item_name_before_edit_ = item->text(0);
            ui->projectTree->editItem(item);
        });

        menu.addAction(act_rename);
    }

    void MainWindow::LoadSettings()
    {
        QSettings s(SETTINGS_FILE_NAME, QSettings::IniFormat);
        restoreGeometry(s.value("windowGeometry").toByteArray());
        restoreState(s.value("windowState").toByteArray());
    }

    void MainWindow::SaveSettings()
    {
        QSettings s(SETTINGS_FILE_NAME, QSettings::IniFormat);
        s.setValue("windowGeometry", saveGeometry());
        s.setValue("windowState", saveState());
    }
}
