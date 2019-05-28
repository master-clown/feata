#pragma once

#include "gui/wgt/basewidget.hpp"


class QLabel;
class QCheckBox;

namespace gui::wgt
{
    class WidgetBool
            : public BaseWidget
    {
    public:
        WidgetBool(QWidget* parent = nullptr);
        WidgetBool(const plug::PluginSettingValue& val,
                   QWidget* parent = nullptr);
       ~WidgetBool() override;

        bool Init(const plug::PluginSettingValue& val) override;

        bool ToPlugSettValue(plug::PluginSettingValue& val) const override;
        bool IsValid() const override;

    private:
        QLabel* desc_lbl_;
        QCheckBox* check_box_ = nullptr;

        using Base = BaseWidget;

        void Clear_();
    };
}
