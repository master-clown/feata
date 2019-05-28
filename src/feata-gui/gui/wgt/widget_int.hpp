#pragma once

#include "gui/wgt/basewidget.hpp"


class QLabel;
class QSpinBox;

namespace gui::wgt
{
    class WidgetInt
            : public BaseWidget
    {
    public:
        WidgetInt(QWidget* parent = nullptr);
        WidgetInt(const plug::PluginSettingValue& val,
                  QWidget* parent = nullptr);
        ~WidgetInt() override;

        bool Init(const plug::PluginSettingValue& val) override;

        bool ToPlugSettValue(plug::PluginSettingValue& val) const override;
        bool IsValid() const override;

    private:
        QLabel* title_lbl_ = nullptr;
        QLabel* desc_lbl_;
        QSpinBox* spin_box_;

        using Base = BaseWidget;

        void Clear_();
    };
}
