#include "plug/pluginsets.hpp"
#include "data/json.hpp"
#include "plug/pluginmgr.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace plug
{
    template<class T, PluginSettingType type>
    static bool SetByType(const bool is_valid,
                          Map<String, PluginSettingValue>& t,
                          const String& name,
                          const T& val);
    template<class T, PluginSettingType type>
    static bool GetByType(const bool is_valid,
                          const Map<String, PluginSettingValue>& t,
                          const String& name,
                          T& out);


    PluginSettings::PluginSettings(const PluginMgr& plug_mgr)
        : plug_mgr_(plug_mgr)
    {}

    PluginSettings::PluginSettings(const PluginMgr& plug_mgr,
                                   const plugid_t pid)
        : PluginSettings(plug_mgr)
    {
        SetPluginId(pid);
        ParseFromPlugin();
    }

    bool PluginSettings::ParseFromPlugin()
    {
        if(!IsValid())
        {
            PrintE("Plugin settings are not associated with a plugin.");
            return false;
        }

        Plugin* pl;
        if(!plug_mgr_.GetById(plug_id_, pl))
        {
            PrintE("Cannot parse settings from a plugin. Associated plugin ID is invalid.");
            return false;
        }

        return ParseFromString_(pl->PluginPtr->ListRequiredSettings());
    }

    void PluginSettings::SetPluginId(const plugid_t pid) { plug_id_ = pid; }

    void PluginSettings::Set(const String& name, const PluginSettingValue& val) { table_[name] = val; }

    bool PluginSettings::Set(const String& name, const bool val)
    { return SetByType<bool, PluginSettingType::Bool>(IsValid(), table_, name, val); }

    bool PluginSettings::Set(const String& name, const int64 val)
    { return SetByType<int64, PluginSettingType::Int>(IsValid(), table_, name, val); }

    bool PluginSettings::Set(const String& name, const real val)
    { return SetByType<real, PluginSettingType::Real>(IsValid(), table_, name, val); }

    bool PluginSettings::Set(const String& name, const String& val)
    { return SetByType<String, PluginSettingType::String>(IsValid(), table_, name, val); }

    bool PluginSettings::Set(const String& name, const PluginSTStrChoice& val)
    { return SetByType<PluginSTStrChoice, PluginSettingType::StrChoice>(IsValid(), table_, name, val); }

    bool PluginSettings::Set(const String& name, const PluginSTGeomSel& val)
    { return SetByType<PluginSTGeomSel, PluginSettingType::GeomSel>(IsValid(), table_, name, val); }

    bool PluginSettings::Set(const String& name, const PluginSTGeomSelStr& val)
    { return SetByType<PluginSTGeomSelStr, PluginSettingType::GeomSelStr>(IsValid(), table_, name, val); }

    bool PluginSettings::SetStrChoiceIdx(const String& name, const int idx)
    {
        if(!IsValid()) return false;

        if(bool con; !(con = Contains(name)) || table_[name].Type != PluginSettingType::StrChoice)
        {
            PrintE("Unable to set string choice index: setting %1%2 does not exist or "
                   "has other type."_qs
                   .arg(name).arg(con ? (":" + TO_STRING(table_[name].Type)) : ""));
            return false;
        }
        auto& sc = std::get<PluginSTStrChoice>(table_[name].Value);
        if(idx < 0 || sc.VariantLst.size() <= idx)
        {
            PrintE("Unable to set string choice index: it is out of range [0, %1]."_qs
                   .arg(sc.VariantLst.size() - 1));
            return false;
        }

        sc.SelectedIdx = idx;
        return true;
    }

    bool PluginSettings::SetFromString(const String& name,
                                       const PluginSettingType type,
                                       const String& str)
    {
        if(!IsValid()) return false;

        String err;
        if(type == PluginSettingType::Bool)
        {
            bool val = false;
            if(str == "1" || str.toLower() == "true") val = true;
            else
            if(str != "0" && str.toLower() != "false")
            {
                err = "unknown Bool value.";
                goto lbl_parse_error;
            }

            return Set(name, val);
        }
        if(type == PluginSettingType::Int)
        {
            bool is_ok;
            int64 val = str.toLongLong(&is_ok);
            if(!is_ok)
            {
                err = "unknown Int value.";
                goto lbl_parse_error;
            }

            return Set(name, val);
        }
        if(type == PluginSettingType::Real)
        {
            bool is_ok;
            int64 val = str.toDouble(&is_ok);
            if(!is_ok)
            {
                err = "unknown Real value.";
                goto lbl_parse_error;
            }

            return Set(name, val);
        }
        if(type == PluginSettingType::String)
        {
            return Set(name, str);
        }
        if(type == PluginSettingType::StrChoice)
        {
            bool is_ok;
            int idx = str.toInt(&is_ok);
            if(!is_ok)
            {
                err = "unknown StrChoice index value ('int' expected).";
                goto lbl_parse_error;
            }

            return SetStrChoiceIdx(name, idx);
        }
        if(type == PluginSettingType::GeomSel ||
           type == PluginSettingType::GeomSelStr)
        {
            const bool is_map = type == PluginSettingType::GeomSelStr;

            const auto lst { str.split("~", String::SkipEmptyParts)};
            if(lst.size() < 2)
            {
                err = "geometry selection must consist of geom enitity type"
                      "and selection list, both separated with '~'.";
                goto lbl_parse_error;
            }

            const auto gent_type {geom::GeomEntityTypeFromString(lst[0])};
            if(gent_type == geom::GEOM_ENT_TYPE_INVALID)
            {
                err = "unknown geometric entity type '%1'."_qs.arg(lst[0]);
                goto lbl_parse_error;
            }

            const auto sel_lst {lst[1].split('$', String::SkipEmptyParts)};
            if(!is_map)
            {
                PluginSTGeomSel sel;
                sel.EntType = gent_type;
                for(const auto& id_str: sel_lst)
                {
                    bool is_ok;
                    const auto id = id_str.toInt(&is_ok);
                    if(!is_ok)
                    {
                        err = "invalid geometry ID value '%1'."_qs.arg(id_str);
                        goto lbl_parse_error;
                    }

                    sel.IdLst.append(id);
                }

                Set(name, sel);
            }
            else
            {
                PluginSTGeomSelStr map;
                map.EntType = gent_type;
                for(const auto& pair_str: sel_lst)
                {
                    bool is_ok;
                    const auto pair = pair_str.split('@', String::SkipEmptyParts);
                    if(pair.size() < 2)
                    {
                        err = "invalid geometry-string pair '%1'."_qs.arg(pair_str);
                        goto lbl_parse_error;
                    }

                    const auto id = pair[0].toInt(&is_ok);
                    if(!is_ok)
                    {
                        err = "invalid geometry ID value '%1' in geometry-string pair."_qs.arg(pair[0]);
                        goto lbl_parse_error;
                    }

                    map.IdStrMap[id] = pair[1];
                }

                Set(name, map);
            }
        }

        return false;

    lbl_parse_error:
        PrintE("Error during 'PluginSettings::SetFromString()' parse of '%1': %2"_qs
               .arg(str).arg(err));
        return false;   // unreachable
    }

    bool PluginSettings::SetFromString(const String& name,
                                       const String& str)
    {
        if(!IsValid()) return false;

        if(!Contains(name))
        {
            PrintE("Error during 'PluginSettings::SetFromString()' parse: "
                   "setting '%1' does not exist"_qs.arg(name));
            return false;
        }

        return SetFromString(name, table_[name].Type, str);
    }

    void PluginSettings::Remove(const String& name) { if(IsValid() && Contains(name)) table_.remove(name); }
    void PluginSettings::Reset(const String& name)  { if(IsValid() && Contains(name)) table_[name].Value = table_[name].DefValue; }
    void PluginSettings::Clear() { table_.clear(); }

    const PluginMgr& PluginSettings::GetPluginMgr() const { return plug_mgr_; }
    plugid_t PluginSettings::GetPluginId() const { return plug_id_; }

    bool PluginSettings::Get(const String& name, PluginSettingValue& val) const
    { if(!IsValid() || !table_.contains(name)) return false; val = table_[name]; return true; }

    bool PluginSettings::GetBool(const String& name, bool& val) const
    { return GetByType<bool, PluginSettingType::Bool>(IsValid(), table_, name, val); }

    bool PluginSettings::GetInt(const String& name, int64& val) const
    { return GetByType<int64, PluginSettingType::Int>(IsValid(), table_, name, val); }

    bool PluginSettings::GetReal(const String& name, real& val) const
    { return GetByType<real, PluginSettingType::Real>(IsValid(), table_, name, val); }

    bool PluginSettings::GetString(const String& name, String& val) const
    { return GetByType<String, PluginSettingType::String>(IsValid(), table_, name, val); }

    bool PluginSettings::GetStrChoice(const String& name, PluginSTStrChoice& val) const
    { return GetByType<PluginSTStrChoice, PluginSettingType::StrChoice>(IsValid(), table_, name, val); }

    bool PluginSettings::GetGeomSel(const String &name, PluginSTGeomSel &val) const
    { return GetByType<PluginSTGeomSel, PluginSettingType::GeomSel>(IsValid(), table_, name, val); }

    bool PluginSettings::GetGeomSelStr(const String &name, PluginSTGeomSelStr &val) const
    { return GetByType<PluginSTGeomSelStr, PluginSettingType::GeomSelStr>(IsValid(), table_, name, val); }

    bool PluginSettings::GetStrChoiceIdx(const String& name, int& idx) const
    {
        if(!IsValid()) return false;

        if(bool con; !(con = Contains(name)) || table_[name].Type != PluginSettingType::StrChoice)
        {
            PrintE("Unable to get string choice index: setting %1%2 does not exist or "
                   "has other type."_qs .arg(name)
                   .arg(con ? (":" + TO_STRING(table_[name].Type)) : ""));
            return false;
        }

        idx = std::get<PluginSTStrChoice>(table_[name].Value).SelectedIdx;
        return true;
    }

    bool PluginSettings::GetToString(const String& name, String& str) const
    {
        if(!IsValid()) return false;

        PluginSettingType type;
        if(!Contains(name, type))
        {
            PrintE("Setting '%1' does not exist"_qs.arg(name));
            return false;
        }

        if(type == PluginSettingType::Bool)
        { bool v; GetBool(name, v); str = v ? "true" : "false"; }
        else
        if(type == PluginSettingType::Int)
        { int64 v; GetInt(name, v); str = TO_STRING(v); }
        else
        if(type == PluginSettingType::Real)
        { real v;  GetReal(name, v); str = TO_STRING(v); }
        else
        if(type == PluginSettingType::String)
        { GetString(name, str); }
        else
        if(type == PluginSettingType::StrChoice)
        { int v; GetStrChoiceIdx(name, v); str = TO_STRING(v); }
        else
        if(type == PluginSettingType::GeomSel)
        {
            PluginSTGeomSel sel;
            GetGeomSel(name, sel);

            if(!sel.IdLst.isEmpty())
            {
                str = TO_STRING(sel.EntType) + "~";
                for(const auto id: sel.IdLst) str += "%1$"_qs.arg(id);
                str.chop(1);
            }
            else str = "";
        }
        else
        if(type == PluginSettingType::GeomSelStr)
        {
            PluginSTGeomSelStr map;
            GetGeomSelStr(name, map);

            const auto keys {map.IdStrMap.keys()};
            if(!keys.isEmpty())
            {
                str = TO_STRING(map.EntType) + "~";
                for(const auto id: keys) str += "%1@%2$"_qs.arg(id).arg(map.IdStrMap[id]);
                str.chop(1);
            }
            else str = "";
        }

        return true;
    }

    StringList PluginSettings::GetParamNames() const { return table_.keys(); }
    Map<String, PluginSettingValue> PluginSettings::ToMap() const { return table_; }

    bool PluginSettings::IsValid() const { return plug_id_ != PLUGIN_ID_NULL; }

    bool PluginSettings::Contains(const String& name) const  { return IsValid() && table_.contains(name); }
    bool PluginSettings::Contains(const String& name,
                                  PluginSettingType& type) const
    {
        if(!Contains(name))
            return false;

        type = table_[name].Type;
        return true;
    }

    bool PluginSettings::ParseFromString_(const String& str)
    {
        using namespace data::json;

        const auto set_lst = data::json::ParseFromString(str).array();
        if(set_lst.isEmpty())
            return false;

        String emsg;
        for(int i_s = 0; i_s < set_lst.size(); ++i_s)
        {
            const auto s {set_lst[i_s].toObject()};
            PluginSettingType type;
            String value;

            if(!s.contains("codename"))
            {
                emsg = "No 'codename' supplied for %1th setting."_qs.arg(i_s);
                goto lbl_parse_error;
            }
            if(!s.contains("fullname"))
            {
                emsg = "No 'fullname' supplied for %1th setting."_qs.arg(i_s);
                goto lbl_parse_error;
            }
            if(!s.contains("type"))
            {
                emsg = "No 'type' supplied for %1th setting."_qs.arg(i_s);
                goto lbl_parse_error;
            }
            if((type = PluginSettingTypeFromString(s["type"].toString()))
                == PluginSettingType::Null)
            {
                emsg = "Invalid type '%1' for %2th setting."_qs.arg(s["type"].toString()).arg(i_s);
                goto lbl_parse_error;
            }
            if((type != PluginSettingType::GeomSel && type != PluginSettingType::GeomSelStr)
               && !s.contains("value"))
            {
                emsg = "No 'value' supplied for %1th setting."_qs.arg(i_s);
                goto lbl_parse_error;
            }
            value = s["value"].toString();

            PluginSettingValue val;
            val.FullName = s["fullname"].toString();
            val.Desc     = s.contains("desc") ? s["desc"].toString() : "-";
            val.Type     = type;
            const auto name = s["codename"].toString();

            if(val.Type == PluginSettingType::Bool)
            {
                val.Value = val.DefValue = s["value"].toBool();
            }
            else
            if(val.Type == PluginSettingType::Int)
            {
                val.Value = val.DefValue = value.toLongLong();
            }
            else
            if(val.Type == PluginSettingType::Real)
            {
                val.Value = val.DefValue = value.toDouble();
            }
            else
            if(val.Type == PluginSettingType::String)
            {
                val.Value = val.DefValue = value;
            }
            else
            if(val.Type == PluginSettingType::StrChoice)
            {
                if(!s.contains("strchoice_lst"))
                {
                    emsg = "No string choice list supplied for parameter '%1'."_qs.arg(name);
                    goto lbl_parse_error;
                }

                const auto sch_lst {s["strchoice_lst"].toArray()};
                if(sch_lst.empty())
                {
                    emsg = "String choice list is empty for parameter '%1'."_qs.arg(name);
                    goto lbl_parse_error;
                }

                const auto idx {value.toInt()};
                if(idx < 0 || sch_lst.size() <= idx)
                {
                    emsg = "Out of range index '%1' for strchoice parameter '%2'."_qs.arg(idx).arg(name);
                    goto lbl_parse_error;
                }

                PluginSTStrChoice sch;
                sch.SelectedIdx = idx;

                for(const auto& str: sch_lst)
                    sch.VariantLst << str.toString();

                val.Value = val.DefValue = sch;
            }
            else
            if(type == PluginSettingType::GeomSel || type == PluginSettingType::GeomSelStr)
            {
                const bool is_map = type != PluginSettingType::GeomSel;

                if(!s.contains("geom_select_type"))
                {
                    emsg = "Geometric entity type is not supplied for parameter '%1'."_qs.arg(name);
                    goto lbl_parse_error;
                }

                const auto gent_type_str {s["geom_select_type"].toString()};
                const auto gent_type {geom::GeomEntityTypeFromString(gent_type_str)};
                if(gent_type == geom::GEOM_ENT_TYPE_INVALID)
                {
                    emsg = "Unknown geometric entity type '%1' for parameter '%2'."_qs
                           .arg(gent_type_str).arg(name);
                    goto lbl_parse_error;
                }

                if(is_map)
                    val.Value = val.DefValue = PluginSTGeomSelStr(gent_type);
                else
                    val.Value = val.DefValue = PluginSTGeomSel(gent_type);
            }

            Set(name, val);
        }

        return true;

    lbl_parse_error:
        PrintE("Error during a parse of plugin parameters from string. " + emsg);
        Clear();
        return false;
    }


    template<class T, PluginSettingType type>
    bool SetByType(const bool is_valid,
                   Map<String, PluginSettingValue>& t,
                   const String& name,
                   const T& val)
    {
        if(!is_valid) return false;

        if(bool con; !(con = t.contains(name)) || t[name].Type != type)
        {
            PrintE("Unable to set setting value: setting '%1:%2' does not exist and/or "
                   "has other type%3."_qs
                   .arg(name).arg(TO_STRING(type))
                   .arg(con ? (" '" + TO_STRING(t[name].Type) + "'") : ""));
            return false;
        }

        std::get<T>(t[name].Value) = val;
        return true;
    }

    template<class T, PluginSettingType type>
    bool GetByType(const bool is_valid,
                   const Map<String, PluginSettingValue>& t,
                   const String& name,
                   T& out)
    {
        if(!is_valid) return false;

        if(bool con; !(con = t.contains(name)) || t[name].Type != type)
        {
            PrintE("Unable to get setting value: setting '%1:%2' does not exist and/or "
                   "has other type%3."_qs
                   .arg(name).arg(TO_STRING(type))
                   .arg(con ? (" '" + TO_STRING(t[name].Type) + "'") : ""));
            return false;
        }

        out = std::get<T>(t[name].Value);
        return true;
    }
}
