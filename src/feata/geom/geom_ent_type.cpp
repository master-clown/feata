#include "geom/geom_ent_type.hpp"


namespace geom
{
    GeomEntityType GeomEntityTypeFromString(const String& str)
    {
        const auto tmp = str.toLower();
        if(tmp == "vertex") return GEOM_ENT_TYPE_VERTEX;
        if(tmp == "edge") return GEOM_ENT_TYPE_EDGE;
        if(tmp == "face") return GEOM_ENT_TYPE_FACE;
        if(tmp == "solid") return GEOM_ENT_TYPE_SOLID;

        return GEOM_ENT_TYPE_INVALID;
    }

    GeomEntityType GeomEntityTypeFromSelect(const gui::SelectEntityType type)
    {
        switch (type)
        {
        case gui::SELECT_ENTITY_TYPE_NONE:   return GEOM_ENT_TYPE_INVALID;
        case gui::SELECT_ENTITY_TYPE_VERTEX: return GEOM_ENT_TYPE_VERTEX;
        case gui::SELECT_ENTITY_TYPE_EDGE:   return GEOM_ENT_TYPE_EDGE;
        case gui::SELECT_ENTITY_TYPE_FACE:   return GEOM_ENT_TYPE_FACE;
        case gui::SELECT_ENTITY_TYPE_SOLID:  return GEOM_ENT_TYPE_SOLID;
        }

        return GEOM_ENT_TYPE_INVALID;
    }

    gui::SelectEntityType GeomEntityTypeToSelect(const GeomEntityType type)
    {
        switch (type)
        {
        case GEOM_ENT_TYPE_INVALID:return gui::SELECT_ENTITY_TYPE_NONE;
        case GEOM_ENT_TYPE_VERTEX: return gui::SELECT_ENTITY_TYPE_VERTEX;
        case GEOM_ENT_TYPE_EDGE:   return gui::SELECT_ENTITY_TYPE_EDGE;
        case GEOM_ENT_TYPE_FACE:   return gui::SELECT_ENTITY_TYPE_FACE;
        case GEOM_ENT_TYPE_SOLID:  return gui::SELECT_ENTITY_TYPE_SOLID;
        }

        return gui::SELECT_ENTITY_TYPE_NONE;
    }
}

template<>
String TO_STRING(const geom::GeomEntityType& type)
{
    switch(type)
    {
    case geom::GEOM_ENT_TYPE_VERTEX:  return "vertex";
    case geom::GEOM_ENT_TYPE_EDGE:    return "edge";
    case geom::GEOM_ENT_TYPE_FACE:    return "face";
    case geom::GEOM_ENT_TYPE_SOLID:   return "solid";
    default: return "";
    }
}
