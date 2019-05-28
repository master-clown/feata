#pragma once

#include "gui/wgt/basewidget.hpp"


class QLabel;
class QComboBox;

namespace gui::wgt
{
    class WidgetStrChoice
            : public BaseWidget
    {
    public:
        WidgetStrChoice(QWidget* parent = nullptr);
        WidgetStrChoice(const plug::PluginSettingValue& val,
                        QWidget* parent = nullptr);
        ~WidgetStrChoice() override;

        bool Init(const plug::PluginSettingValue& val) override;

        bool ToPlugSettValue(plug::PluginSettingValue& val) const override;
        bool IsValid() const override;

    private:
        QLabel* title_lbl_;
        QLabel* desc_lbl_;
        QComboBox* combo_box_ = nullptr;

        using Base = BaseWidget;

        void Clear_();
    };
}
