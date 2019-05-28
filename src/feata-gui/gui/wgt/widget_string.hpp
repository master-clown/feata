#pragma once

#include "gui/wgt/basewidget.hpp"


class QLabel;
class QTextEdit;

namespace gui::wgt
{
    class WidgetString
            : public BaseWidget
    {
    public:
        WidgetString(QWidget* parent = nullptr);
        WidgetString(const plug::PluginSettingValue& val,
                     QWidget* parent = nullptr);
        ~WidgetString() override;

        bool Init(const plug::PluginSettingValue& val) override;

        bool ToPlugSettValue(plug::PluginSettingValue& val) const override;
        bool IsValid() const override;

    private:
        QLabel* title_lbl_;
        QLabel* desc_lbl_;
        QTextEdit* text_edit_ = nullptr;

        using Base = BaseWidget;

        void Clear_();
    };
}
