#include "gui/wgt/widget_strchoice.hpp"
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include "plug/pluginsets_types.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace gui::wgt
{
    WidgetStrChoice::WidgetStrChoice(const plug::PluginSettingValue& val,
                                     QWidget* parent)
        : WidgetStrChoice(parent)
    {
        Init(val);
    }

    WidgetStrChoice::~WidgetStrChoice()
    {
        Clear_();
    }

    bool WidgetStrChoice::Init(const plug::PluginSettingValue& val)
    {
        if(val.Type != plug::PluginSettingType::StrChoice)
        {
            PrintE("Cannot init string choice widget: wrong type '%1'."_qs
                   .arg(TO_STRING(val.Type)));
            return false;
        }
        if(val.FullName.isEmpty())
        {
            PrintE("Cannot init string choice widget: full name is not specified.");
            return false;
        }

        const auto& sch = std::get<plug::PluginSTStrChoice>(val.Value);
        if(sch.VariantLst.empty())
        {
            PrintE("Cannot init string choice widget: list of variants is empty.");
            return false;
        }

        Clear_();

        title_lbl_ = new QLabel(val.FullName);
        title_lbl_->setStyleSheet("font-weight: bold;");
        desc_lbl_ = new QLabel(val.Desc);
        title_lbl_->setWordWrap(true);
        desc_lbl_->setWordWrap(true);

        combo_box_ = new QComboBox();
        combo_box_->addItems(sch.VariantLst);
        combo_box_->setCurrentIndex(sch.SelectedIdx);

        connect(combo_box_, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &BaseWidget::BecameDirty);

        auto lout = new QVBoxLayout(this);
        lout->addWidget(title_lbl_);
        lout->addWidget(combo_box_);
        lout->addStretch();
        lout->addWidget(desc_lbl_);
        lout->addStretch();

        setLayout(lout);

        return true;
    }

    bool WidgetStrChoice::ToPlugSettValue(plug::PluginSettingValue& val) const
    {
        if(!IsValid())
            return false;

#pragma message("Is it really ref?")
        auto& sch = std::get<plug::PluginSTStrChoice>(val.Value);

        sch.SelectedIdx = combo_box_->currentIndex();

        return true;
    }

    bool WidgetStrChoice::IsValid()  const { return title_lbl_ && !title_lbl_->text().isEmpty(); }

    WidgetStrChoice::WidgetStrChoice(QWidget* parent)
        : Base(parent)
    {}

    void WidgetStrChoice::Clear_()
    {
        if(!layout())
            return;

        Base::DestroyLayout(layout());

        combo_box_ = nullptr;
        desc_lbl_ = nullptr;
        title_lbl_ = nullptr;
    }
}
