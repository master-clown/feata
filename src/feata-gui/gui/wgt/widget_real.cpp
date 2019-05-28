#include "gui/wgt/widget_real.hpp"
#include <QLabel>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include "plug/pluginsets_types.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace gui::wgt
{
    WidgetReal::WidgetReal(const plug::PluginSettingValue& val,
                           QWidget* parent)
        : WidgetReal(parent)
    {
        Init(val);
    }

    WidgetReal::~WidgetReal()
    {
        Clear_();
    }

    bool WidgetReal::Init(const plug::PluginSettingValue& val)
    {
        if(val.Type != plug::PluginSettingType::Real)
        {
            PrintE("Cannot init real-number widget: wrong type '%1'."_qs
                   .arg(TO_STRING(val.Type)));
            return false;
        }
        if(val.FullName.isEmpty())
        {
            PrintE("Cannot init real-number widget: full name is not specified.");
            return false;
        }

        Clear_();

        title_lbl_ = new QLabel(val.FullName);
        title_lbl_->setStyleSheet("font-weight: bold;");
        desc_lbl_ = new QLabel(val.Desc);
        title_lbl_->setWordWrap(true);
        desc_lbl_->setWordWrap(true);

        spin_box_ = new QDoubleSpinBox();
        //spin_box_->setAccelerated(true);
        spin_box_->setRange(-DBL_MAX, DBL_MAX);
        spin_box_->setDecimals(9);
        spin_box_->setValue(std::get<real>(val.Value));

        connect(spin_box_, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &BaseWidget::BecameDirty);

        auto lout = new QVBoxLayout(this);
        lout->addWidget(title_lbl_);
        lout->addWidget(spin_box_);
        lout->addStretch();
        lout->addWidget(desc_lbl_);
        lout->addStretch();

        setLayout(lout);

        return true;
    }

    bool WidgetReal::ToPlugSettValue(plug::PluginSettingValue& val) const
    {
        if(!IsValid())
            return false;

        val.Value = spin_box_->value();

        return true;
    }

    bool WidgetReal::IsValid()  const { return title_lbl_ && !title_lbl_->text().isEmpty(); }

    WidgetReal::WidgetReal(QWidget* parent)
        : Base(parent)
    {}

    void WidgetReal::Clear_()
    {
        if(!layout())
            return;

        Base::DestroyLayout(layout());

        spin_box_ = nullptr;
        desc_lbl_ = nullptr;
        title_lbl_ = nullptr;
    }
}
