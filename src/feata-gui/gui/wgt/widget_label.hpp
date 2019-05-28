#pragma once

#include "gui/wgt/basewidget.hpp"
#include "stl/string.hpp"


class QLabel;
class QTextEdit;

namespace gui::wgt
{
    class WidgetLabel
            : public BaseWidget
    {
    public:
        WidgetLabel(QWidget* parent = nullptr);
        WidgetLabel(const plug::PluginSettingValue& val,
                    QWidget* parent = nullptr);
        WidgetLabel(const String& txt,
                    QWidget* parent = nullptr);
        ~WidgetLabel() override;

        bool Init(const plug::PluginSettingValue& val) override;
        bool Init(const String& txt);

        void SetText(const String& txt);

        String GetText() const;
        bool ToPlugSettValue(plug::PluginSettingValue& val) const override;
        bool IsValid() const override;

    private:
        QLabel* lbl_;

        using Base = BaseWidget;

        void Clear_();
    };
}
