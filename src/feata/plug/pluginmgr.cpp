#include "plug/pluginmgr.hpp"
#include "plug/pluginsets.hpp"
#include "plug/services.hpp"
#include "core/shlib_mgr.hpp"
#include "data/json.hpp"
#include "util/filesys.hpp"
#include "util/log.hpp"
#include "defs/error.hpp"


namespace plug
{
    PluginMgr::PluginMgr(core::ShLibMgr& lib_mgr)
        : lib_mgr_(lib_mgr)
    {
        plugin_lst_[0] = new Plugin();
        plugin_lst_[0]->Info.Name = "null";
    }

    PluginMgr::~PluginMgr()
    {
        for(const auto ptr: alloc_lst_)
            service::FreeMemory(ptr);

        const auto ids = GetPluginList();
        for(const auto& id: ids)
        {
            UnloadPlugin(id);
        }
    }

    const Vector<ServiceInfo> PluginMgr::ServiceLst =
    {
        { "log", &service::Log },
        { "logi", &service::LogI },
        { "logw", &service::LogW },
        { "loge", &service::LogE },
        { "init_progress_bar", &service::InitProgressBar },
        { "set_progress_value", &service::SetProgressValue },
        { "hide_progress_bar", &service::HideProgressBar },
        { "get_setting_value", &service::GetSettingValue },
        { "free_output_data", &service::FreeMemory }
    };

    plugid_t PluginMgr::LoadPlugin(const String& file_name)
    {
        using util::logger::PrintE;

        const auto file_meta = file_name + ".meta";
        if(!util::filesys::IsFileExists(file_meta))
        {
            PrintE(String("Unable to find .meta file for a plugin located at \"%1\".").arg(file_name));
            return PLUGIN_ID_NULL;
        }

        const auto meta_data = data::json::ParseFromString(util::filesys::ReadFile(file_meta)).object();
        Plugin* pl = new Plugin();
        pl->Info = {
            meta_data["name"].toString(),
            meta_data["description"].toString(),
            file_name,
            meta_data["version"].toString()
        };
        pl->Type = (meta_data["type"].toString() == "mesher") ?
                        PLUGIN_TYPE_MESHER : PLUGIN_TYPE_SOLVER;

        const auto pl_libname = pl->GetAssociatedLibName();
        if(!lib_mgr_.Load(pl_libname, file_name, true))
        {
            PrintE(String("Unable to find plugin \"%1\" located at \"%2\". "
                          "Error code: %3")
                   .arg(pl->Info.Name).arg(file_name).arg(FeataApiLastError()));
            return PLUGIN_ID_NULL;
        }

        pl->CreateCb = reinterpret_cast<PluginCreateCb>(lib_mgr_[pl_libname]->Lib.FindSymbol("CreatePlugin"));
        pl->FreeCb   = reinterpret_cast<PluginFreeCb>  (lib_mgr_[pl_libname]->Lib.FindSymbol("FreePlugin"));

        if(!pl->CreateCb || !pl->FreeCb)
        {
            PrintE("Unable to find 'create' and/or 'free' routines of the plugin \"%1\" located at \"%2\". "
                   "Error code: %3"_qs.arg(pl->Info.Name).arg(file_name).arg(FeataApiLastError()));
            lib_mgr_.Unload(pl_libname);
            return PLUGIN_ID_NULL;
        }

        if(pl->CreatePlugin(ServiceLst))
        {
            PrintE(String("Unable to create and/or init the plugin \"%1\" located at \"%2\".")
                   .arg(pl->Info.Name).arg(file_name));
            lib_mgr_.Unload(pl_libname);
            return PLUGIN_ID_NULL;
        }

        plugin_lst_[++used_id_] = pl;

        return used_id_;
    }

    bool PluginMgr::UnloadPlugin(const plugid_t id)
    {
        Plugin* pl;
        if(!GetById(id, pl) || pl->IsNull())
            return false;

        pl->FreeCb(pl->PluginPtr);
        lib_mgr_.Unload(pl->GetAssociatedLibName());
        plugin_lst_.remove(id);
        delete pl;

        return true;
    }

    void PluginMgr::AddAllocPtr(const char* ptr) const { alloc_lst_.append(ptr); }
    bool PluginMgr::RemoveAllocPtr(const char* const ptr) const { return alloc_lst_.removeOne(ptr); }

    Plugin* PluginMgr::GetById(const plugid_t id) const
    {
        Plugin* pl;
        if(!GetById(id, pl))
            return nullptr;

        return pl;
    }

    bool PluginMgr::GetById(const plugid_t id, Plugin*& pl) const
    {
        if(!Contains(id))
            return false;

        pl = plugin_lst_[id];

        return true;
    }

    bool PluginMgr::Contains(const plugid_t id) const { return plugin_lst_.contains(id); }
    List<plugid_t> PluginMgr::GetPluginList() const { return plugin_lst_.keys(); }

    plugid_t PluginMgr::FindByName(const String& plug_name) const
    {
        const auto id_lst {GetPluginList()};
        for(const auto& id: id_lst)
            if(plugin_lst_[id]->Info.Name == plug_name)
                return id;

        return PLUGIN_ID_NULL;
    }

    bool PluginMgr::HasAllocPtr(const char* ptr) const { return alloc_lst_.contains(ptr); }
}
