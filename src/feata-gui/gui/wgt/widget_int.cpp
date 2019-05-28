#include "gui/wgt/widget_int.hpp"
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include "plug/pluginsets_types.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace gui::wgt
{
    WidgetInt::WidgetInt(const plug::PluginSettingValue& val,
                         QWidget* parent)
        : WidgetInt(parent)
    {
        Init(val);
    }

    WidgetInt::~WidgetInt()
    {
        Clear_();
    }

    bool WidgetInt::Init(const plug::PluginSettingValue& val)
    {
        if(val.Type != plug::PluginSettingType::Int)
        {
            PrintE("Cannot init integer-number widget: wrong type '%1'."_qs
                   .arg(TO_STRING(val.Type)));
            return false;
        }
        if(val.FullName.isEmpty())
        {
            PrintE("Cannot init integer-number widget: full name is not specified.");
            return false;
        }

        Clear_();

        title_lbl_ = new QLabel(val.FullName);
        title_lbl_->setStyleSheet("font-weight: bold;");
        desc_lbl_ = new QLabel(val.Desc);
        title_lbl_->setWordWrap(true);
        desc_lbl_->setWordWrap(true);

        spin_box_ = new QSpinBox();
        spin_box_->setAccelerated(true);
        spin_box_->setGroupSeparatorShown(true);
        spin_box_->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        spin_box_->setRange(INT_MIN, INT_MAX);
        spin_box_->setValue(std::get<int64>(val.Value));

        connect(spin_box_, QOverload<int>::of(&QSpinBox::valueChanged),
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

    bool WidgetInt::ToPlugSettValue(plug::PluginSettingValue& val) const
    {
        if(!IsValid())
            return false;

        val.Value = (int64)spin_box_->value();

        return true;
    }

    bool WidgetInt::IsValid()  const { return title_lbl_ && !title_lbl_->text().isEmpty(); }

    WidgetInt::WidgetInt(QWidget* parent)
        : Base(parent)
    {}

    void WidgetInt::Clear_()
    {
        if(!layout())
            return;

        Base::DestroyLayout(layout());

        spin_box_ = nullptr;
        desc_lbl_ = nullptr;
        title_lbl_ = nullptr;
    }
}
