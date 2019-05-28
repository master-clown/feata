#include "gui/wgt/widgetsetscont.hpp"
#include <QVBoxLayout>
#include "geom/mdl/convert.hpp"
#include "gui/dlg/mboxdlg.hpp"
#include "gui/wgt/widget_bool.hpp"
#include "gui/wgt/widget_geomsel.hpp"
#include "gui/wgt/widget_geomselstr.hpp"
#include "gui/wgt/widget_int.hpp"
#include "gui/wgt/widget_real.hpp"
#include "gui/wgt/widget_strchoice.hpp"
#include "gui/wgt/widget_string.hpp"
#include "plug/pluginsets.hpp"
#include "proj/glob_events.hpp"
#include "util/log.hpp"


namespace gui::wgt
{
    WidgetSetsCont::WidgetSetsCont(QWidget* parent)
        : Base(parent)
    {}

    WidgetSetsCont::WidgetSetsCont(const plug::PluginSettings& plug_sets,
                                   QWidget* parent)
        : WidgetSetsCont(parent)
    {
        Init(plug_sets);
    }

    WidgetSetsCont::~WidgetSetsCont()
    {
        Clear();
    }

    bool WidgetSetsCont::Init(const plug::PluginSettings& plug_sets)
    {
        using set_t = plug::PluginSettingType;

        Clear();

        if(!plug_sets.IsValid())
        {
            return false;
        }

        String emsg;
        const auto keys {plug_sets.GetParamNames()};

        QVBoxLayout* lout;

        plug::PluginSettingValue sv;
        for(const auto& k: keys)
        {
            plug_sets.Get(k, sv);

            bool is_ptr_selector = false;
            BaseWidget* ptr = nullptr;
            switch (sv.Type)
            {
                case set_t::Bool:       ptr = new WidgetBool(); break;
                case set_t::GeomSel:    ptr = new WidgetGeomSel(); is_ptr_selector = true; break;
                case set_t::GeomSelStr: ptr = new WidgetGeomSelStr(); is_ptr_selector = true; break;
                case set_t::Int:        ptr = new WidgetInt(); break;
                case set_t::Real:       ptr = new WidgetReal(); break;
                case set_t::StrChoice:  ptr = new WidgetStrChoice(); break;
                case set_t::String:     ptr = new WidgetString(); break;
                default: break;
            }

            if(!ptr)
            {
                emsg = "Setting '%1': its type '%2' is not supported in the widget system."_qs
                       .arg(k).arg(TO_STRING(sv.Type));
                goto lbl_error;
            }
            if(!ptr->Init(sv))
            {
                emsg = "Cannot init '%1:%2' setting."_qs.arg(k).arg(TO_STRING(sv.Type));
                goto lbl_error;
            }
            if(is_ptr_selector)
            {
                auto sptr = static_cast<BaseWidgetGeomSel*>(ptr);
                connect(sptr, &BaseWidgetGeomSel::SelectionToggled,
                        this, &WidgetSetsCont::OnSelectionToggled);
            }

            cont_[k] = ptr;
        }

        lout = new QVBoxLayout();
        for(const auto p: cont_)
        {
            connect(p, &BaseWidget::BecameDirty, this, &WidgetSetsCont::OnDirty);
            lout->addWidget(p);
        }

        setLayout(lout);

        return true;

    lbl_error:
        for(const auto p: cont_)
            delete p;
        cont_.clear();

        return false;
    }

    void WidgetSetsCont::Clear()
    {
        auto lout = layout();
        if(!lout)
            return;

        BaseWidget::DestroyLayout(lout);
        cont_.clear();
    }

    void WidgetSetsCont::ToPluginSettings(plug::PluginSettings& plug_sets)
    {
        for(auto it = cont_.cbegin(), end = cont_.cend();
            it != end; ++it)
        {
            if(!plug_sets.Contains(it.key()))
            {
                dlg::MboxCritical("Error", "Plugin setting '%1' was not found."_qs.arg(it.key()));
                exit(1);
            }

            plug::PluginSettingValue val;
            plug_sets.Get(it.key(), val);
            it.value()->ToPlugSettValue(val);
            plug_sets.Set(it.key(), val);
        }
    }

    bool WidgetSetsCont::GetSettValue(const String& name,
                                      plug::PluginSettingValue& val) const
    {
        if(cont_.contains(name))
        {
            util::logger::PrintE("The setting '%1' does not exist."_qs.arg(name));
            return false;
        }

        return cont_[name]->ToPlugSettValue(val);
    }

    void WidgetSetsCont::OnSelectionToggled(const bool is_on)
    {
        auto caller = static_cast<BaseWidgetGeomSel*>(sender());

        if(!is_on)
        {
            proj::glev::GeomEntSelectionDisable.Raise();
            return;
        }

        proj::glev::GeomEntSelectionEnable.Raise(caller,
                                                 geom::GeomEntityTypeToSelect(caller->GetGeomSelType()));

        for(auto wgt: cont_)
        {
            auto obj = dynamic_cast<BaseWidgetGeomSel*>(wgt);
            if(obj && obj != caller)
            {
                obj->SetSelectionState(false);
            }
        }
    }

    void WidgetSetsCont::OnDirty()
    {
        proj::glev::ActiveComponentBecameDirty.Raise();
    }
}
