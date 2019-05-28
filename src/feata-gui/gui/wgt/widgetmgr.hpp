#pragma once

#include "core/comp_def.hpp"
#include "stl/string.hpp"
#include "stl/map.hpp"


namespace plug      { class PluginSettings; }
namespace geom::mdl { struct ConvertToStlInfo; }
namespace gui::wgt  { class WidgetSetsCont;
                      class WidgetTriangulation;
                      class WidgetLabel; }
class QWidget;
class QLabel;
class QGroupBox;
class QPushButton;

namespace gui::wgt
{
    class WidgetMgr
    {
    public:
        WidgetMgr(const QWidget* container,
                  QWidget* title_cont,
                  QLabel* header_title,
                  QPushButton* btn_start,
                  QGroupBox* plugin_sel_cont,
                  QGroupBox* comp_sel_cont);
       ~WidgetMgr();

        bool AddWidgetCont(const core::ComponentPluggable comp_type, const int id);
        bool AddWidgetCont(const core::ComponentPluggable comp_type,
                           const int id,
                           const plug::PluginSettings& plug_sets);
        bool AddWidgetTriang(const int id);
        bool AddWidgetTriang(const int id,
                             const geom::mdl::ConvertToStlInfo& conv_info);
        bool AddWidgetPlug(const int id,
                           const String& plug_info);

        bool RemoveWidgetCont(const core::ComponentPluggable comp_type,
                              const int id);
        bool RemoveWidgetTriang(const int id);
        bool RemoveWidgetPlug(const int id);

        bool ShowWidgetCont(const core::ComponentPluggable comp_type,
                            const int id);
        bool ShowWidgetTriang(const int id);
        bool ShowWidgetPlug(const int id);

        bool ChangeWidgetContSets(const core::ComponentPluggable comp_type,
                                  const int id,
                                  const plug::PluginSettings& plug_sets);

        void HideWidget();
        void HideHeader();

        /**
         * \brief SetHeaderStyle
         *     Convienience function to make the header look in some
         *     standard way for 'comp_type' value
         */
        void SetHeaderStyle(const core::Component comp_type,
                            const String& comp_name);

        // following methods do not clear out argument before editing.
        bool GetPluginSettings(const core::ComponentPluggable comp_type,
                               const int id,
                               plug::PluginSettings& plug_sets) const;
        bool GetTriangInfo(const int id,
                           geom::mdl::ConvertToStlInfo& conv_info) const;

    private:
        Map<core::ComponentPluggable,
            Map<int, WidgetSetsCont*>> widget_cont_lst_;
        Map<int, WidgetTriangulation*> widget_trng_lst_;
        Map<int, WidgetLabel*> widget_plug_lst_;

        const QWidget* container_;
        QWidget* title_cont_;
        QLabel* title_lbl_;
        QPushButton* title_btn_;
        QGroupBox* plugin_sel_cont_;  // container with combobox for plugin selection
        QGroupBox* comp_sel_cont_;

        void SetWidgetContTitleText_(const core::Component comp_type,
                                     const String& name);
        void SetWidgetTriangTitleText_(const String& name);
        void SetTitleVisible_(const bool is_vis);
        void SetButtonText_(const String& name);
        void SetButtonVisible_(const bool is_vis);
    };
}
