#pragma once

#include <variant>
#include "geom/geom_ent_type.hpp"
#include "geom/gid_t.hpp"
#include "stl/map.hpp"
#include "stl/string.hpp"
#include "stl/list.hpp"


namespace plug
{
    // \sa 'TO_STRING()' below
    enum class PluginSettingType
    {
        Null,
        Bool,
        Int,
        Real,
        String,
        StrChoice,
        GeomSel,
        GeomSelStr
    };
    PluginSettingType PluginSettingTypeFromString(const String& s);

    struct PluginSTStrChoice        // ST - setting type
    {
        StringList VariantLst;
        int SelectedIdx = 0;
    };

    struct PluginSTGeomSel
    {
        geom::GeomEntityType EntType;
        List<geom::gid_t> IdLst;

        PluginSTGeomSel() = default;
        PluginSTGeomSel(const geom::GeomEntityType type)
            : EntType(type)
        {}
    };

    struct PluginSTGeomSelStr
    {
        geom::GeomEntityType EntType;
        Map<geom::gid_t, String> IdStrMap;

        PluginSTGeomSelStr() = default;
        PluginSTGeomSelStr(const geom::GeomEntityType type)
            : EntType(type)
        {}
    };

    geom::GeomEntityType GeomSelEntTypeFromString(const String& str);

    struct PluginSettingValue
    {
        std::variant<bool,
                     int64,
                     double,
                     String,
                     PluginSTStrChoice,
                     PluginSTGeomSel,
                     PluginSTGeomSelStr> Value, DefValue;
        String FullName;
        String Desc;
        PluginSettingType Type;

        PluginSettingValue() = default;
        PluginSettingValue(const String& full_name,
                           const String& desc,
                           const PluginSettingType type);

        void SetData(const String& full_name,
                     const String& desc,
                     const PluginSettingType type);
    };


    inline PluginSettingValue::PluginSettingValue(
            const String& full_name,
            const String& desc,
            const PluginSettingType type)
    {
        SetData(full_name, desc, type);
    }


    inline void PluginSettingValue::SetData(const String& full_name,
                                            const String& desc,
                                            const PluginSettingType type)
    {
        FullName = full_name;
        Desc = desc;
        Type = type;
    }
}

template<>
String TO_STRING(const plug::PluginSettingType& type);
