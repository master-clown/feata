#include "plug/plugininfo.hpp"


namespace plug
{
    String PluginInfo::GetEncodedName() const
    {
        return Name + "_" + TO_STRING(Vers);
    }

    String PluginInfo::ToString() const
    {
        return "Name: %1\n\nVersion: %2\n\nFile: %3\n\nDescription: %4"_qs
                .arg(Name).arg(TO_STRING(Vers)).arg(File).arg(Desc);
    }
}
