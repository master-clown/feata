#include "plug/pluginsets_types.hpp"


namespace plug
{
    PluginSettingType PluginSettingTypeFromString(const String& s)
    {
        if(s == "bool") return PluginSettingType::Bool;
        if(s == "int") return PluginSettingType::Int;
        if(s == "real") return PluginSettingType::Real;
        if(s == "string") return PluginSettingType::String;
        if(s == "strchoice") return PluginSettingType::StrChoice;
        if(s == "geom_select") return PluginSettingType::GeomSel;
        if(s == "geom_select_string") return PluginSettingType::GeomSelStr;

        return PluginSettingType::Null;
    }
}

template<>
String TO_STRING(const plug::PluginSettingType& type)
{
    switch(type)
    {
        case plug::PluginSettingType::Bool:         return "bool";
        case plug::PluginSettingType::Int:          return "int";
        case plug::PluginSettingType::Real:         return "real";
        case plug::PluginSettingType::String:       return "string";
        case plug::PluginSettingType::StrChoice:    return "strchoice";
        case plug::PluginSettingType::GeomSel:      return "geom_select";
        case plug::PluginSettingType::GeomSelStr:   return "geom_select_string";
        default: return "";
    }
}
