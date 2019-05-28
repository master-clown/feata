#pragma once

#include "gui/select_defs.hpp"
#include "stl/string.hpp"


namespace geom
{
    enum GeomEntityType
    {
        GEOM_ENT_TYPE_INVALID = -1,
        GEOM_ENT_TYPE_SOLID = 2,
        GEOM_ENT_TYPE_FACE = 4,
        GEOM_ENT_TYPE_EDGE = 6,
        GEOM_ENT_TYPE_VERTEX = 7
    };
    GeomEntityType GeomEntityTypeFromString(const String& str);
    GeomEntityType GeomEntityTypeFromSelect(const gui::SelectEntityType type);
    gui::SelectEntityType GeomEntityTypeToSelect(const GeomEntityType type);
}

template<>
String TO_STRING(const geom::GeomEntityType& type);
