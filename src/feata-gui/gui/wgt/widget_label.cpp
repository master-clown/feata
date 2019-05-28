#include "gui/wgt/widget_label.hpp"
#include <QLabel>
#include <QVBoxLayout>
#include "plug/pluginsets_types.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace gui::wgt
{
    WidgetLabel::WidgetLabel(const plug::PluginSettingValue& val,
                             QWidget* parent)
        : WidgetLabel(parent)
    {
        Init(val);
    }
    WidgetLabel::WidgetLabel(const String& txt,
                             QWidget* parent)
        : WidgetLabel(parent)
    {
        Init(txt);
    }
    WidgetLabel::~WidgetLabel()
    {
        Clear_();
    }

    bool WidgetLabel::Init(const plug::PluginSettingValue& val)
    {
        return Init(std::get<String>(val.Value));
    }

    bool WidgetLabel::Init(const String& txt)
    {
        Clear_();

        lbl_ = new QLabel(txt);
        lbl_->setWordWrap(true);

        auto lout = new QVBoxLayout(this);
        lout->addWidget(lbl_);
        lout->addStretch();

        setLayout(lout);

        return true;
    }

    void WidgetLabel::SetText(const String& txt) { if(IsValid()) lbl_->setText(txt); }

    String WidgetLabel::GetText() const { return IsValid() ? lbl_->text() : ""; }

    bool WidgetLabel::ToPlugSettValue(plug::PluginSettingValue& val) const
    {
        if(!IsValid())
            return false;

        val.Value = lbl_->text();

        return true;
    }

    bool WidgetLabel::IsValid()  const { return lbl_; }

    WidgetLabel::WidgetLabel(QWidget* parent)
        : Base(parent)
    {}

    void WidgetLabel::Clear_()
    {
        if(!layout())
            return;

        Base::DestroyLayout(layout());

        lbl_ = nullptr;
    }
}
