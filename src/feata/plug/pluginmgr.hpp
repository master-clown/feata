#pragma once

#include "stl/map.hpp"
#include "stl/list.hpp"
#include "stl/vector.hpp"
#include "defs/arch.hpp"
#include "plug/plugin.hpp"


namespace core { class ShLibMgr; }

namespace plug
{
    // always contains 'null' plugin
    class PluginMgr
    {
    public:
        PluginMgr(core::ShLibMgr& lib_mgr);
       ~PluginMgr();

        /**
         * \brief LoadPlugin
         *     Adds plugin located at 'file_name'. In addition to the plugin
         *     itself, there must be '.dll.meta' file with the same base name.
         *
         * \returns
         *     If succeeded, ID of the new plugin, otherwise PLUGIN_ID_NULL
         */
        plugid_t LoadPlugin(const String& file_name);
        bool UnloadPlugin(const plugid_t id);

        void AddAllocPtr(const char* ptr) const;
        bool RemoveAllocPtr(const char* const ptr) const;

        Plugin* GetById(const plugid_t id) const;
        bool GetById(const plugid_t id, Plugin*& pl) const;
        bool Contains(const plugid_t id) const;
        List<plugid_t> GetPluginList() const;

        plugid_t FindByName(const String& plug_name) const;

        bool HasAllocPtr(const char* ptr) const;

    private:
        Map<plugid_t, Plugin*> plugin_lst_;
        core::ShLibMgr& lib_mgr_;
        plugid_t used_id_ = PLUGIN_ID_NULL;      // new plugins are assigned '++used_id'

        mutable List<const char*> alloc_lst_;

        static const Vector<ServiceInfo> ServiceLst;
    };
}
