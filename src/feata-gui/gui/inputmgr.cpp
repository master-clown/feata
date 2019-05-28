#include "gui/inputmgr.hpp"


namespace gui
{
    void InputMgr::OnMouseButtonDown(const MouseInput& in)
    {
        SetMouseBtnState(in.Button, true);
        mouse_last_pr_pos_ = in.Pos;
    }

    void InputMgr::OnMouseButtonUp(const MouseInput& in)
    {
        SetMouseBtnState(in.Button, false);
    }

    void InputMgr::OnMouseMove(const geom::Point2i& pnt)
    {
        mouse_cur_pos_ = pnt;
    }

    void InputMgr::OnMouseWheel(const geom::Point2i& pnt,
                                const int delta)
    {

    }

    bool InputMgr::IsMouseBtnDown(const MouseButtonBit btn) const
    {
        return btn != MOUSE_BUTTON_BIT_NONE ? ((mouse_pr_btns_ & btn) == btn) :
                                              (mouse_pr_btns_ == btn);
    }

    const geom::Point2i& InputMgr::GetMouseLastPressPos() const { return mouse_last_pr_pos_; }
    const geom::Point2i& InputMgr::GetMouseCurPos() const       { return mouse_cur_pos_; }

    void InputMgr::SetMouseBtnState(const MouseButtonBit btn,
                                    const bool is_down)
    {
        mouse_pr_btns_ = is_down ? (mouse_pr_btns_ | btn) :
                                   (mouse_pr_btns_ & ~btn);
    }
}
