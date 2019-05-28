#pragma once

#include "geom/point.hpp"
#include "gui/input_defs.hpp"


namespace gui
{
    class InputMgr
    {
    public:
        void OnMouseButtonDown(const MouseInput& in);
        void OnMouseButtonUp(const MouseInput& in);
        void OnMouseMove(const geom::Point2i& pnt);
        void OnMouseWheel(const geom::Point2i& pnt,
                          const int delta);
        bool IsMouseBtnDown(const MouseButtonBit btn) const;
        const geom::Point2i& GetMouseLastPressPos() const;
        const geom::Point2i& GetMouseCurPos() const;

        void SetMouseBtnState(const MouseButtonBit btn,
                              const bool is_down);
    private:
        geom::Point2i mouse_last_pr_pos_;
        geom::Point2i mouse_cur_pos_;
        int mouse_pr_btns_ = 0;
    };
}
