#include "gui/renderer.hpp"
#include <AIS_ColorScale.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_RubberBand.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_NodalColorPrsBuilder.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include "defs/arch.hpp"
#include "geom/geomvis.hpp"
#include "geom/mdl/convert.hpp"
#include "geom/mdl/stepmodel.hpp"
#include "geom/mdl/triangulate.hpp"
#include "geom/rect.hpp"
#include "gui/inputmgr.hpp"
#include "gui/renderlist.hpp"
#include "gui/util/occt_intcontext.hpp"
#include "mesh/meshvis.hpp"
#include "post/nsolmeshvis.hpp"
#include "proj/glob_events.hpp"


#if defined(FEATA_SYSTEM_WIN)
    #include <WNT_Window.hxx>
#elif defined(FEATA_SYSTEM_UNIX)
    #include <Xw_Window.hxx>
#endif

// OCC selection and highlight are the same thing?
// No way to restore selection color after unhighlighting! Repulsive!
//#define EDGE_SELECTED_COLOR Quantity_NOC_BLUE1
//#define FACE_SELECTED_COLOR Quantity_NOC_GRAY
#define EDGE_CHOSEN_SELECTED_COLOR Quantity_NOC_BLUE4
#define FACE_CHOSEN_SELECTED_COLOR Quantity_NOC_GREEN


namespace gui
{
    struct Renderer::Scene
    {
        Handle(Aspect_DisplayConnection) DispConn;
        Handle(OpenGl_GraphicDriver) GlDriver;
        Handle(V3d_Viewer) Viewer;
        Handle(V3d_AmbientLight) LightAmb;
        Handle(V3d_DirectionalLight) LightDirnlMain;
        Handle(Aspect_Window) Wnd;
        Handle(V3d_View) View;
        Handle(OcctInteractiveContext) Context;
        Handle(AIS_RubberBand) SelRect;
        RenderList RenderLst;
        RenderObjType CurrRenObjsType = RENDER_OBJ_TYPE_NONE;
        Map<RenderObjType, Handle(AIS_InteractiveObject)> ObjectLst;
        Handle(AIS_ColorScale) ColorScale;

        struct SelMap
        {
            Map<geom::gid_t, Handle(StdSelect_BRepOwner)> SelLst;
            geom::gid_t UserChosen = geom::GEOM_ID_NULL;
        };
        struct
        {
            Map<const gui::wgt::BaseWidgetGeomSel*, SelMap> SettSelMap;
            const gui::wgt::BaseWidgetGeomSel* CurrentSett;

            SelMap& GetCurrentSelMap() { return SettSelMap[CurrentSett]; }
        } Selection;

        Scene(void* win_handle);

        void UpdateSelectionMap(const SelectEntityType sel_type);
    };

    Renderer::Renderer()
        : input_mgr_(new InputMgr())
    {}

    Renderer::~Renderer()
    {
        SAFE_DEL(scene_);
        SAFE_DEL(input_mgr_);
    }

    bool Renderer::Init(void* win_handle)
    {
        scene_ = new Scene(win_handle);

        InitFollowEvents();

        return true;
    }

    void Renderer::InitFollowEvents()
    {
        auto tmp_cb_id = proj::glev::GeomEntSelectedInLst.AddFollower(
        [this](const geom::gid_t id)
        {
            return OnGeomEntSelInLst(id);
        });
        tmp_cb_id = proj::glev::GeomEntUnselectAll.AddFollower(
        [this]()
        {
            scene_->Context->ClearSelected(true);
            scene_->UpdateSelectionMap(SELECT_ENTITY_TYPE_NONE);
        });
        tmp_cb_id = proj::glev::GeomEntUnselectOne.AddFollower(
        [this](const geom::gid_t id)
        {
            if(id == scene_->Selection.GetCurrentSelMap().UserChosen)
                OnGeomEntSelInLst(geom::GEOM_ID_NULL);

            scene_->Context->AddOrRemoveSelected(scene_->Selection.GetCurrentSelMap().SelLst[id], true);
            scene_->Selection.GetCurrentSelMap().SelLst.remove(id);
        });

        tmp_cb_id = proj::glev::GeomEntSelectionEnable.AddFollower(
        [this](const gui::wgt::BaseWidgetGeomSel* sender,
               const SelectEntityType type)
        {
            if(type == SELECT_ENTITY_TYPE_NONE) return;

            scene_->Context->ClearSelected(true);

            auto& cur_sel = scene_->Selection.SettSelMap[scene_->Selection.CurrentSett = sender];
            for(auto own: cur_sel.SelLst)
                scene_->Context->AddOrRemoveSelected(own, true);

            SetSelectEntityType(type);
        });
        tmp_cb_id = proj::glev::GeomEntSelectionDisable.AddFollower(
        [this]()
        {
            scene_->Context->ClearSelected(true);
            scene_->Selection.CurrentSett = nullptr;

            SetSelectEntityType(SELECT_ENTITY_TYPE_NONE);
        });
    }

    void Renderer::Render()
    {
        //scene_->Context->UpdateCurrentViewer();
        scene_->View->Redraw();
    }

    void Renderer::OnResize(int w, int h)
    {
        VPWidth = w; VPHeight = h;

        scene_->ColorScale->SetHeight(h / 2.0);
        scene_->ColorScale->SetYPosition(h / 4.0);

        scene_->View->MustBeResized();
    }

    RenderObjType Renderer::AddToScene(const RenderObjType obj_lst)
    {
        auto& ren_lst {scene_->RenderLst};
        const auto new_objs = (scene_->CurrRenObjsType ^ obj_lst) & obj_lst;
        RenderObjType res = RENDER_OBJ_TYPE_NONE;

        if(new_objs & RENDER_OBJ_TYPE_SHAPE)
        {
            const auto mdl = ren_lst.GetCurrentGeometry()->GetOCCModel();
            if(mdl)
            {
                scene_->Context->Display(scene_->ObjectLst[RENDER_OBJ_TYPE_SHAPE] = mdl, true);
                res |= RENDER_OBJ_TYPE_SHAPE;
            }
        }
        else
        if(new_objs & RENDER_OBJ_TYPE_TRIAN)
        {
            const auto mdl = ren_lst.GetCurrentTrianGeometry()->GetOCCModel();
            if(mdl)
            {
                scene_->Context->Display(scene_->ObjectLst[RENDER_OBJ_TYPE_TRIAN] = mdl, true);
                res |= RENDER_OBJ_TYPE_TRIAN;
            }
        }
        else
        if(new_objs & RENDER_OBJ_TYPE_MESH)
        {
            const auto msh = ren_lst.GetCurrentMesh();
            if(msh)
            {
                const auto mdl = msh->GetOCCMesh();
                scene_->Context->Display(scene_->ObjectLst[RENDER_OBJ_TYPE_MESH] = mdl, true);
                res |= RENDER_OBJ_TYPE_MESH;
            }
        }
        else
        if(new_objs & RENDER_OBJ_TYPE_SOLU)
        {
            const auto nsol = ren_lst.GetCurrentNodalSol();
            const auto mdl = nsol->GetOCCMesh();
            if(mdl)
            {
                const auto& builder = Handle(MeshVS_NodalColorPrsBuilder)::DownCast(mdl->GetBuilderById(1));
                scene_->ColorScale->SetRange(nsol->GetMinResValue(), nsol->GetMaxResValue());
                scene_->ColorScale->SetNumberOfIntervals(3);
                scene_->ColorScale->SetColors(builder->GetColorMap());

                scene_->Context->Display(scene_->ObjectLst[RENDER_OBJ_TYPE_SOLU] = mdl, true);
                scene_->Context->Display(scene_->ColorScale, true);
                scene_->Context->CurrentViewer()->RedrawImmediate();
                res |= RENDER_OBJ_TYPE_SOLU;
            }
        }

        scene_->CurrRenObjsType |= new_objs;

        return res;
    }

    RenderObjType Renderer::RemFromScene(const RenderObjType obj_lst)
    {
        auto res = RENDER_OBJ_TYPE_NONE;

        if(obj_lst & RENDER_OBJ_TYPE_SHAPE)
        {
            if(auto obj = scene_->ObjectLst.take(RENDER_OBJ_TYPE_SHAPE))
            {
                scene_->Context->Remove(obj, true);
                scene_->UpdateSelectionMap(SELECT_ENTITY_TYPE_NONE);
                res |= RENDER_OBJ_TYPE_SHAPE;
            }
        }
        else
        if(obj_lst & RENDER_OBJ_TYPE_TRIAN)
        {
            if(auto obj = scene_->ObjectLst.take(RENDER_OBJ_TYPE_TRIAN))
            {
                scene_->Context->Remove(obj, true);
                scene_->UpdateSelectionMap(SELECT_ENTITY_TYPE_NONE);
                res |= RENDER_OBJ_TYPE_TRIAN;
            }
        }
        else
        if(obj_lst & RENDER_OBJ_TYPE_MESH)
        {
            if(auto obj = scene_->ObjectLst.take(RENDER_OBJ_TYPE_MESH))
            {
                scene_->Context->Remove(obj, true);
                res |= RENDER_OBJ_TYPE_MESH;
            }
        }
        else
        if(obj_lst & RENDER_OBJ_TYPE_SOLU)
        {
            if(auto obj = scene_->ObjectLst.take(RENDER_OBJ_TYPE_SOLU))
            {
                scene_->Context->Remove(scene_->ColorScale, true);
                scene_->Context->Remove(obj, true);
                res |= RENDER_OBJ_TYPE_SOLU;
            }
        }

        scene_->CurrRenObjsType &= ~obj_lst;

        return res;
    }

    void Renderer::SetCameraView(const cmd::CameraView plane)
    {
        using namespace cmd;
        cam_view_ = ComposeCameraView(plane, cam_view_);

        gp_Trsf tr;
        gp_Vec up_vec, eye_vec;

        if(cam_view_ == CAMERA_VIEW_FREE) return;
        else
        if(cam_view_ <= CAMERA_VIEW_XY_270)
        { eye_vec = {0, 0, 1}; up_vec = {0, 1, 0};
          tr.SetRotation(gp_Ax1({0, 0, 0}, eye_vec), -0.5*M_PI*(cam_view_ - CAMERA_VIEW_XY)); }
        else
        if(cam_view_ <= CAMERA_VIEW_YZ_270)
        { eye_vec = {1, 0, 0}; up_vec = {0, 0, 1};
          tr.SetRotation(gp_Ax1({0, 0, 0}, eye_vec), -0.5*M_PI*(cam_view_ - CAMERA_VIEW_YZ)); }
        else
        if(cam_view_ <= CAMERA_VIEW_XZ_270)
        { eye_vec = {0, 1, 0}; up_vec = {1, 0, 0};
          tr.SetRotation(gp_Ax1({0, 0, 0}, eye_vec), -0.5*M_PI*(cam_view_ - CAMERA_VIEW_XZ)); }
        else
        { eye_vec = {1, 1, 1}; up_vec = {-1, -1, 1}; }

        up_vec.Transform(tr);
        scene_->View->SetAt(0, 0, 0);
        scene_->View->SetEye(eye_vec.X(), eye_vec.Y(), eye_vec.Z());
        scene_->View->SetUp(up_vec.X(), up_vec.Y(), up_vec.Z());
        scene_->View->FitAll(0.1);
    }

    void Renderer::MirrorView()
    {
        if(cam_view_ == cmd::CAMERA_VIEW_FREE ||
           cam_view_ == cmd::CAMERA_VIEW_XYZ)
            return;

        const auto eye = scene_->View->Camera()->Eye();
        scene_->View->SetEye((cmd::IsYZ(cam_view_) ? -1 : 1)*eye.X(),
                             (cmd::IsXZ(cam_view_) ? -1 : 1)*eye.Y(),
                             (cmd::IsXY(cam_view_) ? -1 : 1)*eye.Z());
    }

    void Renderer::SwitchLight(const bool is_on)
    {
        is_on ? scene_->Viewer->SetLightOn(scene_->LightDirnlMain)
              : scene_->Viewer->SetLightOff(scene_->LightDirnlMain);
        scene_->View->Redraw();
    }

    void Renderer::SetViewMode(const cmd::ViewMode mode)
    {
        scene_ ->Context->SetDisplayMode(mode == cmd::VIEW_MODE_SOLID ?
                                             AIS_Shaded : AIS_WireFrame,
                                         true);
    }

    void Renderer::SetSelectCmd(const gui::SelectCmd cmd)
    {
        sel_cmd_ = cmd;
    }

    void Renderer::SetSelectEntityType(const SelectEntityType type)
    {
#pragma message("Really all objects to set?")
        for(const auto& obj: scene_->ObjectLst)
            scene_->Context->SetSelectionModeActive(obj, (int)(sel_type_ =  type), true,
                                                    AIS_SelectionModesConcurrency_Single);
    }

//    void Renderer::ClearSelection()
//    {
//        scene_->Context->ClearSelected(true);
//        scene_->UpdateSelectionMap(SELECT_ENTITY_TYPE_NONE);
//    }

    void Renderer::OnGeomEntSelInLst(const geom::gid_t id)
    {
        auto& cur_sel = scene_->Selection.GetCurrentSelMap();

        if(cur_sel.UserChosen == id)
            return;

        const auto& prs_mgr = scene_->Context->MainPrsMgr();

        if(cur_sel.UserChosen != geom::GEOM_ID_NULL)
        {
            auto own = cur_sel.SelLst[cur_sel.UserChosen];
            //own->Unhilight(prs_mgr, 0);
            own->Unhilight(prs_mgr, 1);
            own->HilightWithColor(prs_mgr, scene_->Context->HighlightStyle(), 0);
            //own->HilightWithColor(prs_mgr, scene_->Context->HighlightStyle(), 1);
        }

        if((cur_sel.UserChosen = id) == geom::GEOM_ID_NULL)
        {
            scene_->Viewer->Redraw();
            return;
        }

        Handle(Prs3d_Drawer) st_edge = new Prs3d_Drawer();
        st_edge->SetColor(EDGE_CHOSEN_SELECTED_COLOR);
        Handle(Prs3d_Drawer) st_face = new Prs3d_Drawer();
        st_face->SetColor(FACE_CHOSEN_SELECTED_COLOR);

        auto own = cur_sel.SelLst[id];
        own->HilightWithColor(prs_mgr, st_edge, 0);
        own->HilightWithColor(prs_mgr, st_face, 1);

        scene_->Viewer->Redraw();
    }

    InputMgr&   Renderer::GetInputMgr()   { return *input_mgr_; }
    RenderList& Renderer::GetRenderList() { return scene_->RenderLst; }

    SelectCmd Renderer::GetSelectCmd() const                { return sel_cmd_; }
    SelectEntityType Renderer::GetSelectEntityType() const  { return sel_type_; }

    RenderObjType Renderer::GetCurrentRenObjType() const
    { return scene_->CurrRenObjsType; }

    void Renderer::OnMouseButtonDown(const MouseInput& in)
    {
        input_mgr_->OnMouseButtonDown(in);

        if(input_mgr_->IsMouseBtnDown(MOUSE_BUTTON_BIT_L))
        {
            if(sel_cmd_ == gui::SELECT_CMD_RECT_ADD ||
               sel_cmd_ == gui::SELECT_CMD_RECT_REM)
            {
                scene_->SelRect->SetRectangle(in.Pos[0], VPHeight - in.Pos[1],
                                              in.Pos[0], VPHeight - in.Pos[1]);
                scene_->Context->Display(scene_->SelRect, false);
                scene_->Context->RectSelectInit();
            }
            else
            {
                scene_->View->StartRotation(in.Pos[0], in.Pos[1]);
                mouse_act_ |= MOUSE_ACTION_BIT_ROTATE;
                cam_view_ = cmd::CAMERA_VIEW_FREE;
            }
        }
        else
        if(input_mgr_->IsMouseBtnDown(MOUSE_BUTTON_BIT_M))
        {
            mouse_act_ |= MOUSE_ACTION_BIT_ZOOM;
            mouse_zoom_last_ = in.Pos;
        }
        else
        if(input_mgr_->IsMouseBtnDown(MOUSE_BUTTON_BIT_R))
        {
            mouse_act_ |= MOUSE_ACTION_BIT_PAN;
            mouse_pan_last_ = in.Pos;
        }
    }

    void Renderer::OnMouseMove(const geom::Point2i& pnt,
                               const KeyboardModif kb_mods)
    {
        UNUSED_PARAM(kb_mods);

        input_mgr_->OnMouseMove(pnt);

        if((sel_cmd_ == SELECT_CMD_RECT_ADD || sel_cmd_ == SELECT_CMD_RECT_REM)
           && input_mgr_->IsMouseBtnDown(MOUSE_BUTTON_BIT_L))
        {
            const auto origin { input_mgr_->GetMouseLastPressPos() };
            scene_->SelRect->SetRectangle(origin[0], VPHeight - origin[1], pnt[0], VPHeight - pnt[1]);
            scene_->Context->Redisplay(scene_->SelRect, false);
            scene_->Context->CurrentViewer()->RedrawImmediate();

            if(sel_cmd_ == SELECT_CMD_RECT_ADD)
                scene_->Context->RectSelectAdd(origin[0], origin[1], pnt[0], pnt[1], scene_->View, true);
            else
                scene_->Context->RectSelectRemove(origin[0], origin[1], pnt[0], pnt[1], scene_->View, true);
        }
        else
        if(sel_cmd_ == SELECT_CMD_SINGLE && input_mgr_->IsMouseBtnDown(MOUSE_BUTTON_BIT_NONE))
        {
            scene_->Context->MoveTo(pnt[0], pnt[1], scene_->View, true);
        }

        if(mouse_act_ & MOUSE_ACTION_BIT_ROTATE)
        {
            scene_->View->Rotation(pnt[0], pnt[1]);
        }
        if(mouse_act_ & MOUSE_ACTION_BIT_ZOOM)
        {
            scene_->View->Zoom(pnt[1], pnt[1], mouse_zoom_last_[1], mouse_zoom_last_[1]);
            mouse_zoom_last_ = pnt;
        }
        if(mouse_act_ & MOUSE_ACTION_BIT_PAN)
        {
            scene_->View->Pan(pnt[0] - mouse_pan_last_[0], -pnt[1] + mouse_pan_last_[1]);
            mouse_pan_last_ = pnt;
        }
    }

    void Renderer::OnMouseButtonUp(const MouseInput& in,
                                   const bool was_click_single)
    {
        if(in.Button == MOUSE_BUTTON_BIT_L)
        {
            if(sel_cmd_ == SELECT_CMD_RECT_ADD ||
               sel_cmd_ == SELECT_CMD_RECT_REM)
            {
                scene_->Context->Erase(scene_->SelRect, false);
                scene_->Context->RectSelectFinish();
                scene_->UpdateSelectionMap(sel_type_);
            }
            if(sel_cmd_ == SELECT_CMD_SINGLE && was_click_single)
            {
                scene_->Context->ShiftSelect(true);
                scene_->UpdateSelectionMap(sel_type_);
            }
            scene_->Context->CurrentViewer()->RedrawImmediate();

            mouse_act_ &= ~MOUSE_ACTION_BIT_ROTATE;
        }
        else
        if(in.Button == MOUSE_BUTTON_BIT_M)
        {
            mouse_act_ &= ~MOUSE_ACTION_BIT_ZOOM;
        }
        else
        if(in.Button == MOUSE_BUTTON_BIT_R)
        {
            mouse_act_ &= ~MOUSE_ACTION_BIT_PAN;
        }

        input_mgr_->OnMouseButtonUp(in);       // this call must be AFTER those if's!
    }

    void Renderer::OnMouseWheel(const geom::Point2i& pnt,
                                const int delta)
    {
        input_mgr_->OnMouseWheel(pnt, delta);
    }

    Renderer::Scene::Scene(void* win_handle)
    {
        DispConn = new Aspect_DisplayConnection();
        GlDriver = new OpenGl_GraphicDriver(DispConn);
        Viewer = new V3d_Viewer(GlDriver);
        LightAmb = new V3d_AmbientLight();
            Viewer->AddLight(LightDirnlMain = new V3d_DirectionalLight
                                              (V3d_XposYnegZneg, Quantity_NOC_WHITE, true));
            LightAmb->SetIntensity(2);
            Viewer->AddLight(LightAmb);
            Viewer->SetLightOn();
            Viewer->SetDefaultBgGradientColors(Quantity_NOC_BLUEVIOLET,
                                               Quantity_NOC_GRAY,
                                               Aspect_GFM_VER);
        View = Viewer->CreateView();
            View->MustBeResized();
            View->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);
#if defined(FEATA_SYSTEM_WIN)
        Wnd = new WNT_Window((HWND)win_handle);
#elif defined(FEATA_SYSTEM_UNIX)
        Wnd = new Xw_Window(...);
#endif
        View->SetWindow(Wnd);
        if (!Wnd->IsMapped()) Wnd->Map();
        Context = new OcctInteractiveContext(Viewer);
            Context->SetDisplayMode(AIS_DisplayMode::AIS_Shaded, Standard_True);
            Context->DefaultDrawer()->SetFaceBoundaryDraw(true);
            Context->DefaultDrawer()->FaceBoundaryAspect()->SetColor(Quantity_NOC_BLACK);
            Context->DefaultDrawer()->FaceBoundaryAspect()->SetWidth(1.5);
        SelRect = new AIS_RubberBand();
            SelRect->SetLineType(Aspect_TypeOfLine::Aspect_TOL_DASH);
            SelRect->SetFilling(Quantity_NOC_BLUE1, 0.75);
        ColorScale = new AIS_ColorScale();
            ColorScale->SetHueRange(0, 230);        // default are swapped, wtf
            ColorScale->SetColorType(Aspect_TOCSD_USER);
            ColorScale->SetBreadth(25);
            ColorScale->SetXPosition(20);
            int w, h; Wnd->Size(w, h);
            ColorScale->SetHeight(h / 2.0);
            ColorScale->SetYPosition(h / 4.0);
            ColorScale->SetSmoothTransition(true);
            ColorScale->SetTextHeight(16);
            ColorScale->SetZLayer(Graphic3d_ZLayerId_TopOSD);
            ColorScale->SetTransformPersistence(Graphic3d_TMF_2d, gp_Pnt (-1,-1,0));
    }

    void Renderer::Scene::UpdateSelectionMap(const SelectEntityType sel_type)
    {
        List<const TopoDS_Shape*> sh_lst;
        List<Handle(StdSelect_BRepOwner)> own_lst;
        for(Context->InitSelected(); Context->MoreSelected(); Context->NextSelected())
        {
            auto sh_own = Handle(StdSelect_BRepOwner)::DownCast(Context->SelectedOwner());

            if(!sh_own)
                continue;

            sh_lst.append(&sh_own->Shape());
            own_lst.append(sh_own);
        }

        List<geom::gid_t> id_lst;
        if(sel_type != SELECT_ENTITY_TYPE_NONE)
        {
            const auto geom_vis = RenderLst.GetCurrentGeometry();

            if(geom_vis)
            switch (sel_type)
            {
                case SELECT_ENTITY_TYPE_VERTEX: id_lst = geom_vis->GetVertexId(sh_lst); break;
                case SELECT_ENTITY_TYPE_EDGE:   id_lst = geom_vis->GetEdgeId(sh_lst); break;
                case SELECT_ENTITY_TYPE_FACE:   id_lst = geom_vis->GetFaceId(sh_lst); break;
                case SELECT_ENTITY_TYPE_SOLID:  id_lst = geom_vis->GetSolidId(sh_lst); break;
                default: break;
            }
        }

        auto& cur_sel = Selection.GetCurrentSelMap();

        cur_sel.SelLst.clear();
        for(int i = 0; i < id_lst.size(); ++i)
            if(id_lst[i] != geom::GEOM_ID_NULL)
                cur_sel.SelLst[id_lst[i]] = own_lst[i];

        if(!cur_sel.SelLst.contains(cur_sel.UserChosen))
            cur_sel.UserChosen = geom::GEOM_ID_NULL;

        proj::glev::GeomEntSelectionUpdated.Raise(cur_sel.SelLst.keys());
    }
}
