#include "gui/wgt/widgetmgr.hpp"
//#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include "geom/mdl/convert.hpp"
#include "gui/wgt/widgetsetscont.hpp"
#include "gui/wgt/widget_label.hpp"
#include "gui/wgt/widget_triang.hpp"
#include "plug/pluginsets.hpp"


namespace gui::wgt
{
    WidgetMgr::WidgetMgr(const QWidget* container,
                         QWidget* title_cont,
                         QLabel* header_title,
                         QPushButton* btn_start,
                         QGroupBox* plugin_sel_cont,
                         QGroupBox* comp_sel_cont)
    {
        assert((container_ = container) &&
               (title_cont_ = title_cont) &&
               (title_lbl_ = header_title) &&
               (title_btn_ = btn_start) &&
               (plugin_sel_cont_ = plugin_sel_cont) &&
               (comp_sel_cont_ = comp_sel_cont) &&
               "Widget controls are not supplied.");

        for(auto i = core::COMPONENT_PLUGGABLE_MESH;
            i < core::COMPONENT_PLUGGABLE_ENUM_COUNT;
            i = core::ComponentPluggable((int)i + 1))
        {
            widget_cont_lst_[i] = {};
        }
    }

    WidgetMgr::~WidgetMgr()
    {
        HideWidget();

        for(auto ptr: widget_plug_lst_) delete ptr;
        for(auto ptr: widget_trng_lst_) delete ptr;
        for(const auto& map: widget_cont_lst_)
            for(auto ptr: map)
                delete ptr;
    }

    bool WidgetMgr::AddWidgetCont(const core::ComponentPluggable comp_type, const int id)
    {
        if(widget_cont_lst_[comp_type].contains(id))
            return false;

        widget_cont_lst_[comp_type][id] = new WidgetSetsCont();
        return true;
    }

    bool WidgetMgr::AddWidgetCont(const core::ComponentPluggable comp_type,
                                  const int id,
                                  const plug::PluginSettings& plug_sets)
    {
        if(widget_cont_lst_[comp_type].contains(id))
            return false;

        widget_cont_lst_[comp_type][id] = new WidgetSetsCont(plug_sets);
        return true;
    }

    bool WidgetMgr::AddWidgetTriang(const int id)
    {
        return AddWidgetTriang(id, {});
    }

    bool WidgetMgr::AddWidgetTriang(const int id,
                                    const geom::mdl::ConvertToStlInfo& conv_info)
    {
        if(widget_trng_lst_.contains(id))
            return false;

        widget_trng_lst_[id] = new WidgetTriangulation(conv_info);
        return true;
    }

    bool WidgetMgr::AddWidgetPlug(const int id,
                                  const String& plug_info)
    {
        if(widget_plug_lst_.contains(id))
            return false;

        widget_plug_lst_[id] = new WidgetLabel(plug_info);
        return true;
    }

    bool WidgetMgr::RemoveWidgetCont(const core::ComponentPluggable comp_type,
                                     const int id)
    {
        if(widget_cont_lst_[comp_type].contains(id))
        {
            delete widget_cont_lst_[comp_type].take(id);
            return true;
        }

        return false;
    }

    bool WidgetMgr::RemoveWidgetTriang(const int id)
    {
        if(widget_trng_lst_.contains(id))
        {
            delete widget_trng_lst_.take(id);
            return true;
        }

        return false;
    }

    bool WidgetMgr::RemoveWidgetPlug(const int id)
    {
        if(widget_plug_lst_.contains(id))
        {
            delete widget_plug_lst_.take(id);
            return true;
        }

        return false;
    }

    bool WidgetMgr::ShowWidgetCont(const core::ComponentPluggable comp_type,
                                   const int id)
    {
        if(!widget_cont_lst_[comp_type].contains(id))
            return false;

        HideWidget();
        container_->layout()->addWidget(widget_cont_lst_[comp_type][id]);
        widget_cont_lst_[comp_type][id]->setVisible(true);

        return true;
    }

    bool WidgetMgr::ShowWidgetTriang(const int id)
    {
        if(!widget_trng_lst_.contains(id))
            return false;

        HideWidget();
        container_->layout()->addWidget(widget_trng_lst_[id]);
        widget_trng_lst_[id]->setVisible(true);

        return true;
    }

    bool WidgetMgr::ShowWidgetPlug(const int id)
    {
        if(!widget_plug_lst_.contains(id))
            return false;

        HideWidget();
        HideHeader();
        container_->layout()->addWidget(widget_plug_lst_[id]);
        widget_plug_lst_[id]->setVisible(true);

        return true;
    }

    bool WidgetMgr::ChangeWidgetContSets(const core::ComponentPluggable comp_type,
                                         const int id,
                                         const plug::PluginSettings& plug_sets)
    {
        if(!widget_cont_lst_[comp_type].contains(id))
            return false;

        return widget_cont_lst_[comp_type][id]->Init(plug_sets);
    }

    void WidgetMgr::HideWidget()
    {
        if(auto ptr = container_->layout()->takeAt(0))
        {
            ptr->widget()->setVisible(false);
            delete ptr;
        }
    }

    void WidgetMgr::HideHeader()
    {
        SetTitleVisible_(false);
        SetButtonVisible_(false);
        plugin_sel_cont_->setVisible(false);
        plugin_sel_cont_->setVisible(false);
    }

    void WidgetMgr::SetHeaderStyle(const core::Component comp_type,
                                   const String& comp_name)
    {
        if(comp_type != core::COMPONENT_TRNG)
            SetWidgetContTitleText_(comp_type, comp_name);
        else
            SetWidgetTriangTitleText_(comp_name);

        bool pl_sel_vis = false;
        bool cp_sel_vis = false;
        String btn_txt;
        String cp_sel_title;
        switch (comp_type)
        {
        case core::COMPONENT_TRNG: btn_txt = "Build"; break;
        case core::COMPONENT_MESH: btn_txt = "Build"; pl_sel_vis = true; break;
        case core::COMPONENT_SOLV:
            btn_txt = "Solve";
            pl_sel_vis = cp_sel_vis = true;
            cp_sel_title = "Mesh";
            break;
        case core::COMPONENT_POST:
//            btn_txt = "Plot";
//            pl_sel_vis = cp_sel_vis = true;
//            cp_sel_title = "Solution";
            break;
        default: btn_txt = "";
        }

        SetButtonText_(btn_txt);
        SetTitleVisible_(true);
        SetButtonVisible_(!title_btn_->text().isEmpty());
        plugin_sel_cont_->setVisible(pl_sel_vis);
        comp_sel_cont_->setVisible(cp_sel_vis);
        comp_sel_cont_->setTitle(cp_sel_title);
    }

    void WidgetMgr::SetWidgetContTitleText_(const core::Component comp_type,
                                            const String& name)
    {
        const auto tr { name.trimmed() };
        title_lbl_->setText("%1%2"_qs
                               .arg(core::GetComponentPrefixName(comp_type))
                               .arg((comp_type == core::COMPONENT_GEOM ?
                                         "" : " \"" + tr + "\"")));
    }

    void WidgetMgr::SetWidgetTriangTitleText_(const String& name)
    {
        return SetWidgetContTitleText_(core::COMPONENT_TRNG, name);
    }

    void WidgetMgr::SetTitleVisible_(const bool is_vis)
    {
        if(is_vis)
            title_cont_->setVisible(true);
        else if(!title_btn_->isVisible())
            title_cont_->setVisible(false);

        title_lbl_->setVisible(is_vis);
    }

    void WidgetMgr::SetButtonText_(const String& name)
    {
        title_btn_->setText(name);
    }

    void WidgetMgr::SetButtonVisible_(const bool is_vis)
    {
        if(is_vis)
            title_cont_->setVisible(true);
        else if(!title_lbl_->isVisible())
            title_cont_->setVisible(false);

        title_btn_->setVisible(is_vis);
    }

    bool WidgetMgr::GetPluginSettings(const core::ComponentPluggable comp_type,
                                      const int id,
                                      plug::PluginSettings& plug_sets) const
    {
        if(!widget_cont_lst_[comp_type].contains(id))
            return false;

        widget_cont_lst_[comp_type][id]->ToPluginSettings(plug_sets);
        return true;
    }

    bool WidgetMgr::GetTriangInfo(const int id,
                                  geom::mdl::ConvertToStlInfo& conv_info) const
    {
        if(!widget_trng_lst_.contains(id))
            return false;

        widget_trng_lst_[id]->ToConvertStlInfo(conv_info);
        return true;
    }
}
