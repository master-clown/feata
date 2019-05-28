#pragma once

#include "stl/string.hpp"
#include "defs/common.hpp"


namespace plug
{
    struct Version
    {
        uint16 Major;
        uint16 Minor;
        uint16 Build;
        uint16 Revis;   // revision

        Version() = default;
        Version(uint64 encoded_ver);
        Version(uint16 major,
                uint16 minor,
                uint16 build,
                uint16 revision);
        Version(const String& str);

        uint64 AsEncoded() const;

        bool Parse(const String& str);
    };
}

template<>
String TO_STRING(const plug::Version& ver);
