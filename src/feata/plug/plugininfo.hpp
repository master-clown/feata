#pragma once

#include "plug/version.hpp"
#include "stl/string.hpp"


namespace plug
{
    struct PluginInfo
    {
        String Name;
        String Desc;    // descrition
        String File;
        Version Vers;

        String GetEncodedName() const;
        String ToString() const;
    };
}
