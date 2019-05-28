#include "gui/wgt/widget_triang.hpp"
#include <QVBoxLayout>
#include "geom/mdl/convert.hpp"
#include "gui/wgt/widget_bool.hpp"
#include "gui/wgt/widget_real.hpp"
#include "plug/pluginsets_types.hpp"
#include "proj/glob_events.hpp"

#define SETTING_COUNT 4


namespace gui::wgt
{
    WidgetTriangulation::WidgetTriangulation(QWidget* parent)
        : Base(parent)
    {
        InitGui_({});   // the arg has default-valued fields
    }

    // do not call the standart ctor from here
    WidgetTriangulation::WidgetTriangulation(const geom::mdl::ConvertToStlInfo& conv_info,
                                             QWidget* parent)
        : Base(parent)
    {
        Init(conv_info);
    }

    WidgetTriangulation::~WidgetTriangulation()
    {
        ClearGui_();
    }

    void WidgetTriangulation::Init(const geom::mdl::ConvertToStlInfo& conv_info)
    {
        InitGui_(conv_info);
    }

    void WidgetTriangulation::ToConvertStlInfo(geom::mdl::ConvertToStlInfo& conv_info)
    {
        plug::PluginSettingValue val;

        wgt_lst_[0]->ToPlugSettValue(val);
        conv_info.LinearDeflection = std::get<real>(val.Value);
        wgt_lst_[1]->ToPlugSettValue(val);
        conv_info.AngularDeflection = std::get<real>(val.Value);
        wgt_lst_[2]->ToPlugSettValue(val);
        conv_info.IsRelative = std::get<bool>(val.Value);
        wgt_lst_[3]->ToPlugSettValue(val);
        conv_info.Parallelism = std::get<bool>(val.Value);
    }

    void WidgetTriangulation::InitGui_(const geom::mdl::ConvertToStlInfo& conv_info)
    {
        ClearGui_();
        plug::PluginSettingValue val;

        val.SetData("Linear deflection", "", plug::PluginSettingType::Real);
        val.Value = val.DefValue = conv_info.LinearDeflection;
        wgt_lst_[0] = new WidgetReal(val);

        val.SetData("Angular deflection", "", plug::PluginSettingType::Real);
        val.Value = val.DefValue = conv_info.AngularDeflection;
        wgt_lst_[1] = new WidgetReal(val);

        val.SetData("Are deflections relative",
                    "If on, deflection used for discretization of "
                    "each edge would equal to <linear deflection> * <size of edge>. "
                    "Deflection used for the faces will be the maximum deflection "
                    "of their edges.",
                    plug::PluginSettingType::Bool);
        val.Value = val.DefValue = conv_info.IsRelative;
        wgt_lst_[2] = new WidgetBool(val);

        val.SetData("Triangulate in parallel",
                    "", plug::PluginSettingType::Bool);
        val.Value = val.DefValue = conv_info.Parallelism;
        wgt_lst_[3] = new WidgetBool(val);

        auto lout = new QVBoxLayout(this);
        for(int i = 0; i < SETTING_COUNT; ++i)
        {
            connect(wgt_lst_[i], &BaseWidget::BecameDirty, this, &WidgetTriangulation::OnDirty);
            lout->addWidget(wgt_lst_[i]);
        }

        setLayout(lout);
    }

    void WidgetTriangulation::ClearGui_()
    {
        auto lout = layout();
        if(!lout)
            return;

        BaseWidget::DestroyLayout(lout);
        for(int i = 0; i < SETTING_COUNT; ++i) wgt_lst_[i] = nullptr;
    }

    void WidgetTriangulation::OnDirty()
    {
        proj::glev::ActiveComponentBecameDirty.Raise();
    }
}
