#include "core/comp_def.hpp"


#define ENUM_TO_STR_CASE(eval) case (eval): return (#eval)


namespace core
{
    String GetComponentPrefixName(const Component comp)
    {
        switch (comp)
        {
            case COMPONENT_GEOM: return "Geometry";
            case COMPONENT_TRNG: return "Polygonalization";
            case COMPONENT_MESH: return "Mesh";
            case COMPONENT_SOLV: return "Solver";
            case COMPONENT_POST: return "Results";
            default: return "";
        }
    }

    String GetComponentPluggablePrefixName(const ComponentPluggable comp)
    {
        switch (comp)
        {
            case COMPONENT_PLUGGABLE_ENUM_COUNT: return "";
            default: return GetComponentPrefixName(Component((int)comp + COMPONENT_MESH));
        }
    }
}

template<>
String TO_STRING<core::Component>(const core::Component& val)
{
    switch (val)
    {
        ENUM_TO_STR_CASE(core::COMPONENT_GEOM);
        ENUM_TO_STR_CASE(core::COMPONENT_MESH);
        ENUM_TO_STR_CASE(core::COMPONENT_SOLV);
        ENUM_TO_STR_CASE(core::COMPONENT_POST);
        default: return "";
    }
}

template<>
String TO_STRING<core::ComponentPluggable>(const core::ComponentPluggable& val)
{
    switch (val)
    {
        ENUM_TO_STR_CASE(core::COMPONENT_PLUGGABLE_MESH);
        ENUM_TO_STR_CASE(core::COMPONENT_PLUGGABLE_SOLV);
        ENUM_TO_STR_CASE(core::COMPONENT_PLUGGABLE_POST);
        default: return "";
    }
}
