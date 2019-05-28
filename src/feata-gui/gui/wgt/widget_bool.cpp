#include "gui/wgt/widget_bool.hpp"
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include "plug/pluginsets_types.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace gui::wgt
{
    WidgetBool::WidgetBool(QWidget* parent)
        : Base(parent)
    {}

    WidgetBool::WidgetBool(const plug::PluginSettingValue& val,
                           QWidget* parent)
        : WidgetBool(parent)
    {
        Init(val);
    }

    WidgetBool::~WidgetBool()
    {
        Clear_();
    }

    bool WidgetBool::Init(const plug::PluginSettingValue& val)
    {
        if(val.Type != plug::PluginSettingType::Bool)
        {
            PrintE("Cannot init boolean widget: wrong type '%1'."_qs
                   .arg(TO_STRING(val.Type)));
            return false;
        }
        if(val.FullName.isEmpty())
        {
            PrintE("Cannot init boolean widget: full name is not specified.");
            return false;
        }

        Clear_();

        check_box_ = new QCheckBox(val.FullName);
        check_box_->setText(val.FullName);
        check_box_->setChecked(std::get<bool>(val.Value));

        desc_lbl_ = new QLabel(val.Desc);
        desc_lbl_->setWordWrap(true);

        connect(check_box_, &QCheckBox::stateChanged, this, &BaseWidget::BecameDirty);

        auto lout = new QVBoxLayout(this);
        lout->addWidget(check_box_);
        lout->addStretch();
        lout->addWidget(desc_lbl_);
        lout->addStretch();

        setLayout(lout);

        return true;
    }

    bool WidgetBool::ToPlugSettValue(plug::PluginSettingValue& val) const
    {
        if(!IsValid())
            return false;

        val.Value = check_box_->isChecked();

        return true;
    }

    bool WidgetBool::IsValid()  const { return check_box_ && !check_box_->text().isEmpty(); }

    void WidgetBool::Clear_()
    {
        if(!layout())
            return;

        Base::DestroyLayout(layout());

        check_box_ = nullptr;
        desc_lbl_ = nullptr;
    }
}
