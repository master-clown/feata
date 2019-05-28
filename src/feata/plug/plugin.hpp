#pragma once

#include "plug/iplugin.h"
#include "plug/plugid_t.hpp"
#include "plug/plugininfo.hpp"
#include "plug/service.h"
#include "stl/vector.hpp"


namespace plug
{
    class PluginSettings;

    enum PluginType
    {
        PLUGIN_TYPE_NONE,
        PLUGIN_TYPE_MESHER,
        PLUGIN_TYPE_SOLVER
    };

    struct Plugin
    {
        IPlugin*        PluginPtr = nullptr;
        PluginInfo      Info;
        PluginCreateCb  CreateCb = nullptr;
        PluginFreeCb    FreeCb = nullptr;
        PluginType      Type = PLUGIN_TYPE_NONE;
        plugid_t        Id = PLUGIN_ID_NULL;

        int CreatePlugin(const Vector<ServiceInfo>& service_lst);

        String GetAssociatedLibName() const;
        bool IsNull() const;
    };
}
