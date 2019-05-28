#pragma once

#include "gui/wgt/basewidget.hpp"


class QLabel;
class QDoubleSpinBox;

namespace gui::wgt
{
    class WidgetReal
            : public BaseWidget
    {
    public:
        WidgetReal(QWidget* parent = nullptr);
        WidgetReal(const plug::PluginSettingValue& val,
                   QWidget* parent = nullptr);
       ~WidgetReal() override;

        bool Init(const plug::PluginSettingValue& val) override;

        bool ToPlugSettValue(plug::PluginSettingValue& val) const override;
        bool IsValid() const override;

    private:
        QLabel* title_lbl_ = nullptr;
        QLabel* desc_lbl_;
        QDoubleSpinBox* spin_box_;

        using Base = BaseWidget;

        void Clear_();
    };
}
