#include "gui/wgt/widget_string.hpp"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include "plug/pluginsets_types.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace gui::wgt
{
    WidgetString::WidgetString(const plug::PluginSettingValue& val,
                               QWidget* parent)
        : WidgetString(parent)
    {
        Init(val);
    }

    WidgetString::~WidgetString()
    {
        Clear_();
    }

    bool WidgetString::Init(const plug::PluginSettingValue& val)
    {
        if(val.Type != plug::PluginSettingType::String)
        {
            PrintE("Cannot init string widget: wrong type '%1'."_qs
                   .arg(TO_STRING(val.Type)));
            return false;
        }
        if(val.FullName.isEmpty())
        {
            PrintE("Cannot init string widget: full name is not specified.");
            return false;
        }

        Clear_();

        title_lbl_ = new QLabel(val.FullName);
        title_lbl_->setStyleSheet("font-weight: bold;");
        desc_lbl_ = new QLabel(val.Desc);
        title_lbl_->setWordWrap(true);
        desc_lbl_->setWordWrap(true);

        text_edit_ = new QTextEdit(std::get<String>(val.Value));

        connect(text_edit_, &QTextEdit::textChanged, this, &BaseWidget::BecameDirty);

        auto lout = new QVBoxLayout(this);
        lout->addWidget(title_lbl_);
        lout->addWidget(text_edit_);
        lout->addStretch();
        lout->addWidget(desc_lbl_);
        lout->addStretch();

        setLayout(lout);

        return true;
    }

    bool WidgetString::ToPlugSettValue(plug::PluginSettingValue& val) const
    {
        if(!IsValid())
            return false;

        val.Value = text_edit_->toPlainText();

        return true;
    }

    bool WidgetString::IsValid()  const { return title_lbl_ && !title_lbl_->text().isEmpty(); }

    WidgetString::WidgetString(QWidget* parent)
        : Base(parent)
    {}

    void WidgetString::Clear_()
    {
        if(!layout())
            return;

        Base::DestroyLayout(layout());

        text_edit_ = nullptr;
        desc_lbl_ = nullptr;
        title_lbl_ = nullptr;
    }
}
