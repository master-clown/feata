#pragma once

#include "geom/geom_ent_type.hpp"
#include "geom/gid_t.hpp"
#include "geom/point.hpp"
#include "gui/input_defs.hpp"
#include "gui/select_defs.hpp"
#include "gui/toolbar_cmd.hpp"
#include "gui/renderobjtype.hpp"
#include "stl/string.hpp"


namespace gui
{
    enum MouseActionBit
    {
        MOUSE_ACTION_BIT_NONE = 0,
        MOUSE_ACTION_BIT_PAN = 0b001,
        MOUSE_ACTION_BIT_ROTATE = 0b010,
        MOUSE_ACTION_BIT_ZOOM = 0b100
    };

    class InputMgr;
    class RenderList;

    class Renderer
    {
    public:
        Renderer();
       ~Renderer();

        bool Init(void* win_handle);
        void InitFollowEvents();
        void Render();
        void OnResize(int w, int h);

        /**
         * \brief AddToScene, RemFromScene
         *     Add and remove already initilized objects
         *
         * \return
         *     List of objects, which were actually added/removed
         */
        RenderObjType AddToScene(const RenderObjType obj_lst);
        RenderObjType RemFromScene(const RenderObjType obj_lst);

        void SetCameraView(const cmd::CameraView plane);
        void MirrorView();
        void SwitchLight(const bool is_on);
        void SetViewMode(const cmd::ViewMode mode);

        void SetSelectCmd(const gui::SelectCmd cmd);
        void SetSelectEntityType(const SelectEntityType type);
        //void ClearSelection();

        // when user (un-) selects something from list of selected entities
        void OnGeomEntSelInLst(const geom::gid_t id);

        InputMgr&   GetInputMgr();
        RenderList& GetRenderList();

        SelectCmd GetSelectCmd() const;
        SelectEntityType GetSelectEntityType() const;

        RenderObjType GetCurrentRenObjType() const;

        void OnMouseButtonDown(const MouseInput& in);
        void OnMouseMove(const geom::Point2i& pnt,
                         const KeyboardModif kb_mods);
        void OnMouseButtonUp(const MouseInput& in,
                             const bool was_click_single);
        void OnMouseWheel(const geom::Point2i& pnt,
                          const int delta);

    private:
        struct Scene;

        Scene* scene_;
        int mouse_act_ = MOUSE_ACTION_BIT_NONE;
        InputMgr* input_mgr_;
        geom::Point2i mouse_zoom_last_;
        geom::Point2i mouse_pan_last_;
        uint16 VPWidth;
        uint16 VPHeight;
        cmd::CameraView cam_view_ = cmd::CAMERA_VIEW_FREE;

        SelectCmd sel_cmd_;
        SelectEntityType sel_type_;
    };
}
