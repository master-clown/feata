#include "plug/plugin.hpp"
#include "plug/mesherplugin.h"
#include "plug/solverplugin.h"


namespace plug
{
    int Plugin::CreatePlugin(const Vector<ServiceInfo>& service_lst)
    {
        if(Type == PLUGIN_TYPE_NONE)
            return -1;
        else
        if(Type == PLUGIN_TYPE_MESHER)
            PluginPtr = static_cast<MesherPlugin*>(CreateCb());
        else
        if(Type == PLUGIN_TYPE_SOLVER)
            PluginPtr = static_cast<SolverPlugin*>(CreateCb());

        if(!PluginPtr)
            return -2;

        return PluginPtr->Init(&service_lst[0], service_lst.size());
    }

    String Plugin::GetAssociatedLibName() const { return "plugin_" + Info.GetEncodedName(); }

    bool Plugin::IsNull() const { return !PluginPtr || !CreateCb || !FreeCb; }
}
