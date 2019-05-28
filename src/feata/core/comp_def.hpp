#pragma once

#include "stl/string.hpp"


namespace core
{
    enum Component
    {
        COMPONENT_GEOM,
        COMPONENT_TRNG,
        COMPONENT_MESH,
        COMPONENT_SOLV,
        COMPONENT_POST,         // postprocess
        COMPONENT_ENUM_COUNT    // might be used as invalid value
    };
    String GetComponentPrefixName(const Component comp);

    enum ComponentPluggable
    {
        COMPONENT_PLUGGABLE_MESH,
        COMPONENT_PLUGGABLE_SOLV,
        COMPONENT_PLUGGABLE_POST,
        COMPONENT_PLUGGABLE_ENUM_COUNT
    };
    String GetComponentPluggablePrefixName(const ComponentPluggable comp);
}

template<>
String TO_STRING<core::Component>(const core::Component& val);
template<>
String TO_STRING<core::ComponentPluggable>(const core::ComponentPluggable& val);
