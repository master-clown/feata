#pragma once

#include "gui/wgt/basewidget.hpp"
#include "stl/string.hpp"
#include "stl/map.hpp"


namespace plug      { class PluginSettings; }


namespace gui::wgt
{
    /**
     * \details The WidgetSetsCont class
     *     It's used to contain every other widget to represent
     *     plugin settings structure.
     */
    class WidgetSetsCont : public QWidget
    {
    public:
        WidgetSetsCont(QWidget* parent = nullptr);
        WidgetSetsCont(const plug::PluginSettings& plug_sets,
                        QWidget* parent = nullptr);
       ~WidgetSetsCont() override;

        bool Init(const plug::PluginSettings& plug_sets);
        void Clear();
        void ToPluginSettings(plug::PluginSettings& plug_sets); // if already contains a setting, updates Value

        bool GetSettValue(const String& name,
                          plug::PluginSettingValue& val) const;

    public slots:
        void OnSelectionToggled(const bool is_on);
        void OnDirty();

    private:
        Map<String, BaseWidget*> cont_;

        using Base = QWidget;

        Q_OBJECT
    };
}
