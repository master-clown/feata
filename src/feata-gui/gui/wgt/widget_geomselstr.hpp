#pragma once

#include "gui/wgt/basewidgetgeomsel.hpp"
#include "geom/geom_ent_type.hpp"
#include "util/event_id.hpp"


namespace gui { class TableWidget; }

class QGroupBox;
class QLabel;

namespace gui::wgt
{
    class WidgetGeomSelStr
            : public BaseWidgetGeomSel
    {
    public:
        WidgetGeomSelStr(QWidget* parent = nullptr);
        WidgetGeomSelStr(const plug::PluginSettingValue& val,
                         QWidget* parent = nullptr);
        ~WidgetGeomSelStr() override;

        bool Init(const plug::PluginSettingValue& val) override;
        void SetSelectionState(const bool is_on) override;

        bool ToPlugSettValue(plug::PluginSettingValue& val) const override;
        bool IsValid() const override;

    protected:
        QLabel* title_lbl_;
        QLabel* desc_lbl_;
        QGroupBox* group_box_;
        TableWidget* table_;

        void hideEvent(QHideEvent* ev) override;

    private:
        event_id eid_sel_changed_ = EVENT_ID_INVALID;
        bool delete_flag = false;       // hotfix

        using Base = BaseWidgetGeomSel;

        void Clear_();
    };
}
