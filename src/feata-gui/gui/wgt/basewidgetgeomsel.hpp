#pragma once

#include "gui/wgt/basewidget.hpp"
#include "geom/geom_ent_type.hpp"


namespace gui::wgt
{
    class BaseWidgetGeomSel
            : public BaseWidget
    {
    public:
        BaseWidgetGeomSel(QWidget* parent = nullptr);

        virtual void SetSelectionState(const bool is_on);
        virtual void SetGeomSelType(const geom::GeomEntityType geom_type);

        virtual bool GetSelectionState() const;
        virtual geom::GeomEntityType GetGeomSelType() const;

    signals:
        /**
         * \brief SelectionToggled
         *     Emit it every time the selection is toggled.
         */
        void SelectionToggled(const bool is_on);

    protected:
        bool selection_state_ = false;
        geom::GeomEntityType geom_type_ = geom::GEOM_ENT_TYPE_INVALID;

    private:
        Q_OBJECT

        using Base = BaseWidget;
    };
}


// impl
namespace gui::wgt
{
    inline BaseWidgetGeomSel::BaseWidgetGeomSel(QWidget* parent)
        : BaseWidget(parent)
    {}

    inline void BaseWidgetGeomSel::SetSelectionState(const bool is_on) { selection_state_ = is_on; }
    inline void BaseWidgetGeomSel::SetGeomSelType(const geom::GeomEntityType geom_type)
    { geom_type_ = geom_type; }

    inline bool BaseWidgetGeomSel::GetSelectionState() const { return selection_state_; }
    inline geom::GeomEntityType BaseWidgetGeomSel::GetGeomSelType() const
    { return geom_type_; }
}
