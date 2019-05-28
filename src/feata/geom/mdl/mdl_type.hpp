#pragma once

#include "stl/string.hpp"


namespace geom::mdl
{
    enum MdlType
    {
        MDL_TYPE_NONE,
        MDL_TYPE_STEP,
        MDL_TYPE_STL
    };

    MdlType MdlTypeByFilename(const String& file);
    bool IsPolygonBased(const MdlType type);
}
