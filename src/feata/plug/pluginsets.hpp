#pragma once

#include "plug/plugid_t.hpp"
#include "plug/pluginsets_types.hpp"
#include "stl/map.hpp"
#include "stl/string.hpp"


namespace plug { class PluginMgr; }

namespace plug
{
    class PluginSettings
    {
    public:
        PluginSettings(const PluginMgr& plug_mgr);
        PluginSettings(const PluginMgr& plug_mgr,
                       const plugid_t pid);

        // Set plugin manager and plugin ID beforehand
        bool ParseFromPlugin();

        void SetPluginId(const plugid_t pid);

        void Set(const String& name, const PluginSettingValue& val);
        bool Set(const String& name, const bool val);
        bool Set(const String& name, const int64 val);
        bool Set(const String& name, const real val);
        bool Set(const String& name, const String& val);
        bool Set(const String& name, const PluginSTStrChoice& val);
        bool Set(const String& name, const PluginSTGeomSel& val);
        bool Set(const String& name, const PluginSTGeomSelStr& val);
        bool SetStrChoiceIdx(const String& name, const int idx);

        /**
         * \brief SetFromString
         *     Parses a value from 'str', casts it to 'type' and
         *     assigns it to 'name' setting.
         * \returns
         *     'false' if casting fails.
         */
        bool SetFromString(const String& name,
                           const PluginSettingType type,
                           const String& str);

        /**
         * \brief SetFromString
         *     The same as previous, only type is taken
         *     from already contained 'name' setting.
         *
         * \returns
         *     'false' if there's no 'name' setting yet
         *     or casting fails.
         */
        bool SetFromString(const String& name,
                           const String& str);

        void Remove(const String& name);
        void Reset(const String& name);     // reset the value to default
        void Clear();

        const PluginMgr& GetPluginMgr() const;
        plugid_t GetPluginId() const;

        bool Get(const String& name, PluginSettingValue& val) const;
        bool GetBool(const String& name, bool& val) const;
        bool GetInt(const String& name, int64& val) const;
        bool GetReal(const String& name, real& val) const;
        bool GetString(const String& name, String& val) const;
        bool GetStrChoice(const String& name, PluginSTStrChoice& val) const;
        bool GetStrChoiceIdx(const String& name, int& idx) const;
        bool GetGeomSel(const String& name, PluginSTGeomSel& val) const;
        bool GetGeomSelStr(const String& name, PluginSTGeomSelStr& val) const;

        /**
         * \brief GetToString
         *     Inverse of 'SetFromString'. Returns 'name' setting
         *     in string representation.
         */
        bool GetToString(const String& name, String& str) const;

        StringList GetParamNames() const;
        Map<String, PluginSettingValue> ToMap() const;

        bool IsValid() const;

        bool Contains(const String& name) const;
        bool Contains(const String& name, PluginSettingType& type) const;

    private:
        Map<String, PluginSettingValue> table_;
        const PluginMgr& plug_mgr_;
        plugid_t plug_id_ = PLUGIN_ID_NULL;

        /**
         * \brief ParseFromString
         *     Rules of string representation are declared in
         *     'plug/iplugin.h' for 'ListRequiredSettings()'.
         *
         *     Does not clear current settings before loading new ones.
         */
        bool ParseFromString_(const String& str);
    };
}
